#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class OneKnobSaturatorAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    OneKnobSaturatorAudioProcessorEditor (OneKnobSaturatorAudioProcessor&);
    ~OneKnobSaturatorAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    OneKnobSaturatorAudioProcessor& audioProcessor;

    juce::Slider driveSlider;
    juce::Label driveLabel;
    juce::Label titleLabel;
    juce::Label brandLabel;
    juce::Label subtitleLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneKnobSaturatorAudioProcessorEditor)
};