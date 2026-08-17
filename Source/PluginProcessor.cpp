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

    // Manopola DRIVE (0.0 -> 100.0%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("DRIVE", 1),
        "Drive",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f
    ));

    // Selettore MODE (0: Warm Tape, 1: Tube, 2: Diode Clipper)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID ("MODE", 1),
        "Saturation Mode",
        juce::StringArray { "Warm Tape", "Tube Saturation", "Diode Clipper" },
        0
    ));

    return layout;
}

void OneKnobSaturatorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumInputChannels());

    // Configurazione filtro High-Pass 24 dB/oct (4° ordine) fisso a 90 Hz
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

    // Lettura dei parametri
    float driveNormalized = apvts.getRawParameterValue("DRIVE")->load() / 100.0f; // Range: 0.0 -> 1.0
    int mode = static_cast<int>(apvts.getRawParameterValue("MODE")->load());

    // Calcolo guadagno di Drive (fino a +30 dB)
    float driveGainDB = driveNormalized * 30.0f;
    float driveGainLinear = juce::Decibels::decibelsToGain(driveGainDB);

    // Dynamic AutoGain Compensation
    float autoGainComp = 1.0f / std::pow(driveGainLinear, 0.70f);

    // 1. Processo di Saturazione canale per canale
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float inputSample = channelData[sample];
            float driven = inputSample * driveGainLinear;
            float saturated = driven;

            switch (mode)
            {
                case 0: // Warm Tape Saturation (Soft Tanh)
                    saturated = std::tanh(driven);
                    break;

                case 1: // Tube Saturation (Asimmetrica, ricca di armoniche pari)
                    if (driven > 0.0f)
                        saturated = 1.0f - std::exp(-driven);
                    else
                        saturated = -std::tanh(std::abs(driven));
                    break;

                case 2: // Diode Clipper (Clipping aggressivo con soft-knee)
                    {
                        float x = driven;
                        if (x > 1.2f)        saturated = 1.0f;
                        else if (x < -1.2f)  saturated = -1.0f;
                        else                 saturated = x - (0.333f * std::pow(x, 3.0f));
                    }
                    break;

                default:
                    saturated = std::tanh(driven);
                    break;
            }

            // Applicazione AutoGain
            channelData[sample] = saturated * autoGainComp;
        }
    }

    // 2. Low-Cut Post-Saturazione (Fisso a 90 Hz, 24 dB/oct)
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