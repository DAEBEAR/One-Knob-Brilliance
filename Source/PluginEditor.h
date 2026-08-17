#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "BinaryData.h"

class CustomKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomKnobLookAndFeel()
    {
        knobImage = juce::ImageCache::getFromMemory (BinaryData::knob_oneknob_png, BinaryData::knob_oneknob_pngSize);
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider&) override
    {
        if (! knobImage.isValid())
            return;

        const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        
        // Calcolo delle dimensioni e del centro del bounds della manopola JUCE
        const float destWidth = static_cast<float> (width);
        const float destHeight = static_cast<float> (height);
        const float centreX = static_cast<float> (x) + destWidth * 0.5f;
        const float centreY = static_cast<float> (y) + destHeight * 0.5f;

        const float imgW = static_cast<float> (knobImage.getWidth());
        const float imgH = static_cast<float> (knobImage.getHeight());

        // Calcola la scala per adattare l'immagine PNG alla dimensione dello slider
        const float scale = juce::jmin (destWidth / imgW, destHeight / imgH);

        // Trasformazione: centra l'origine dell'immagine -> scala -> ruota -> trasla al centro dello slider
        juce::AffineTransform transform = juce::AffineTransform::translation (-imgW * 0.5f, -imgH * 0.5f)
                                           .scaled (scale)
                                           .rotated (angle)
                                           .translated (centreX, centreY);

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