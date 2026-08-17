#include "PluginProcessor.h"
#include "PluginEditor.h"

OneKnobSaturatorAudioProcessor::OneKnobSaturatorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#else
     :
#endif
    apvts (*this, nullptr, "Parameters", createParameterLayout())
{
}

OneKnobSaturatorAudioProcessor::~OneKnobSaturatorAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout OneKnobSaturatorAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Singolo parametro DRIVE (0% -> 100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("DRIVE", 1),
        "Drive",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f
    ));

    return layout;
}

void OneKnobSaturatorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumInputChannels());

    // Filtro HPF 24 dB/oct (4° ordine) fisso a 90 Hz
    *lowCutFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 90.0f, 0.707f);
    lowCutFilter.prepare(spec);
    lowCutFilter.reset();
}

void OneKnobSaturatorAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OneKnobSaturatorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void OneKnobSaturatorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    float driveNormalized = apvts.getRawParameterValue("DRIVE")->load() / 100.0f; // 0.0 -> 1.0

    // Gain di drive fino a +24 dB (più che sufficiente per saturare il nastro senza rompere l'audio)
    float driveGainDB = driveNormalized * 24.0f;
    float driveGainLinear = juce::Decibels::decibelsToGain(driveGainDB);

    // CALCOLO AUTOGAIN CORRETTO PER WARM TAPE (tanh):
    // Per segnali trascurabili (Drive=0), AutoGain = 1.0
    // Man mano che il drive aumenta, compensa la crescita esponenziale del segnale mantenendo unitario il livello di uscita percepito.
    float expectedGain = std::tanh(driveGainLinear) / driveGainLinear;
    if (driveGainLinear < 1.001f) expectedGain = 1.0f;
    
    // Fattore di compensazione calibrato
    float autoGainComp = 1.0f / (driveGainLinear * expectedGain);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float inputSample = channelData[sample];
            
            // 1. Amplificazione d'ingresso
            float driven = inputSample * driveGainLinear;
            
            // 2. Warm Tape Saturation (Saturazione Soft Nastro con asimmetria leggera)
            float saturated = std::tanh(driven + 0.05f * driven * driven);

            // 3. Compensazione AutoGain
            channelData[sample] = saturated * autoGainComp;
        }
    }

    // 4. Low-Cut 24 dB/oct @ 90Hz Post-Saturazione
    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    lowCutFilter.process(context);
}

juce::AudioProcessorEditor* OneKnobSaturatorAudioProcessor::createEditor()
{
    return new OneKnobSaturatorAudioProcessorEditor (*this);
}

void OneKnobSaturatorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void OneKnobSaturatorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OneKnobSaturatorAudioProcessor();
}