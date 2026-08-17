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
        // Forziamo la decodifica dell'immagine mantenendo il canale Alpha (ARGB)
        auto loadedImage = juce::ImageCache::getFromMemory (BinaryData::knob_oneknob_png, BinaryData::knob_oneknob_pngSize);
        knobImage = loadedImage.convertedToFormat (juce::Image::ARGB);
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider&) override
    {
        if (! knobImage.isValid())
            return;

        const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        
        const float destW = static_cast<float> (width);
        const float destH = static_cast<float> (height);
        const float centreX = static_cast<float> (x) + destW * 0.5f;
        const float centreY = static_cast<float> (y) + destH * 0.5f;

        const float imgW = static_cast<float> (knobImage.getWidth());
        const float imgH = static_cast<float> (knobImage.getHeight());

        const float scale = juce::jmin (destW / imgW, destH / imgH);

        // Disegno con interpolazione bilineare trasparente
        g.setImageResamplingQuality (juce::Graphics::highResamplingQuality);

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