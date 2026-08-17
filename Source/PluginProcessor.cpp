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

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("DRIVE", 1),
        "Drive",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f
    ));

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

    float driveNormalized = apvts.getRawParameterValue("DRIVE")->load() / 100.0f;
    int mode = static_cast<int>(apvts.getRawParameterValue("MODE")->load());

    // Base drive fino a +36 dB
    float baseDriveDB = driveNormalized * 36.0f;
    float modeGainBoostDB = 0.0f;

    if (mode == 1) modeGainBoostDB = 6.0f;       // Tube boost
    else if (mode == 2) modeGainBoostDB = 12.0f;  // Diode boost per clipping marcato

    float totalDriveGainLinear = juce::Decibels::decibelsToGain(baseDriveDB + modeGainBoostDB);

    // AutoGain ricalibrato in base alla risposta dinamica
    float autoGainComp = 1.0f / std::pow(totalDriveGainLinear, 0.82f);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float inputSample = channelData[sample];
            float driven = inputSample * totalDriveGainLinear;
            float saturated = driven;

            switch (mode)
            {
                case 0: // WARM TAPE: Compressione morbida e saturazione analogica nastro
                {
                    float x = driven * 0.8f;
                    // Curva asimmetrica leggera per simulare il nastro
                    float tapeSim = std::tanh(x + 0.1f * x * x);
                    saturated = tapeSim * 1.1f;
                    break;
                }

                case 1: // TUBE SATURATION: Armoniche pari marcate e suono valvolare caldo
                {
                    float x = driven;
                    if (x > 0.0f)
                        saturated = 1.5f * (1.0f - std::exp(-x));
                    else
                        saturated = -1.2f * (1.0f - std::exp(x));
                    break;
                }

                case 2: // DIODE CLIPPER: Clipping duro con distorsione metallica/aggressiva
                {
                    float x = driven * 1.5f;
                    // Hard-knee diode wave shaping
                    if (x > 1.0f)       saturated = 0.85f + 0.15f * std::tanh((x - 1.0f) * 2.0f);
                    else if (x < -1.0f) saturated = -0.85f + 0.15f * std::tanh((x + 1.0f) * 2.0f);
                    else                saturated = x - (0.33f * std::pow(x, 3.0f));
                    
                    saturated *= 1.15f;
                    break;
                }

                default:
                    saturated = std::tanh(driven);
                    break;
            }

            channelData[sample] = saturated * autoGainComp;
        }
    }

    // Low-Cut 24 dB/oct @ 90Hz post-saturazione
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