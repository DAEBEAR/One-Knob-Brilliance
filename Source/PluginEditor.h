#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "BinaryData.h"

// Class LookAndFeel personalizzata per ruotare l'immagine della manopola
class CustomKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomKnobLookAndFeel()
    {
        knobImage = juce::ImageCache::getFromMemory (BinaryData::knob_oneknob_png, BinaryData::knob_oneknob_pngSize);
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider& slider) override
    {
        if (! knobImage.isValid())
            return;

        const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        const float radius = static_cast<float> (juce::jmin (width, height)) * 0.5f;
        const float centreX = static_cast<float> (x) + static_cast<float> (width) * 0.5f;
        const float centreY = static_cast<float> (y) + static_cast<float> (height) * 0.5f;

        juce::AffineTransform transform = juce::AffineTransform::rotation (angle, static_cast<float> (knobImage.getWidth()) * 0.5f, static_cast<float> (knobImage.getHeight()) * 0.5f)
                                           .translated (centreX - static_cast<float> (knobImage.getWidth()) * 0.5f,
                                                       centreY - static_cast<float> (knobImage.getHeight()) * 0.5f);

        g.drawImageTransformed (knobImage, transform, true);
    }

private:
    juce::Image knobImage;
};

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
    CustomKnobLookAndFeel customKnobLAF;
    juce::Image backgroundImage;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneKnobSaturatorAudioProcessorEditor)
};