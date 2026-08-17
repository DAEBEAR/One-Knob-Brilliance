#include "PluginProcessor.h"
#include "PluginEditor.h"

OneKnobSaturatorAudioProcessorEditor::OneKnobSaturatorAudioProcessorEditor (OneKnobSaturatorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Caricamento dello sfondo aggiornato a formato PNG
    backgroundImage = juce::ImageCache::getFromMemory (BinaryData::background_oneknob_png, BinaryData::background_oneknob_pngSize);

    driveSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    driveSlider.setLookAndFeel (&customKnobLAF);
    
    // Rotazione calibrata per allinearsi all'arco 0 - MAX
    driveSlider.setRotaryParameters (juce::MathConstants<float>::pi * 1.22f, 
                                     juce::MathConstants<float>::pi * 2.78f, true);
    
    addAndMakeVisible (driveSlider);

    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "DRIVE", driveSlider);

    setSize (420, 588);
}

OneKnobSaturatorAudioProcessorEditor::~OneKnobSaturatorAudioProcessorEditor()
{
    driveSlider.setLookAndFeel (nullptr);
}

void OneKnobSaturatorAudioProcessorEditor::paint (juce::Graphics& g)
{
    if (backgroundImage.isValid())
    {
        g.drawImage (backgroundImage, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit);
    }
    else
    {
        g.fillAll (juce::Colours::black);
    }
}

void OneKnobSaturatorAudioProcessorEditor::resized()
{
    // Posizionamento della manopola centrato sull'arco
    const int knobSize = static_cast<int>(getWidth() * 0.54f);
    const int knobX = (getWidth() - knobSize) / 2;
    const int knobY = static_cast<int>(getHeight() * 0.50f);

    driveSlider.setBounds (knobX, knobY, knobSize, knobSize);
}