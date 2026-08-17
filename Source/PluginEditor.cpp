#include "PluginProcessor.h"
#include "PluginEditor.h"

OneKnobSaturatorAudioProcessorEditor::OneKnobSaturatorAudioProcessorEditor (OneKnobSaturatorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    backgroundImage = juce::ImageCache::getFromMemory (BinaryData::background_oneknob_png, BinaryData::background_oneknob_pngSize);

    driveSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    driveSlider.setLookAndFeel (&customKnobLAF);
    
    // Rotazione da 0 a MAX
    driveSlider.setRotaryParameters (juce::MathConstants<float>::pi * 1.25f, 
                                     juce::MathConstants<float>::pi * 2.75f, true);
    
    addAndMakeVisible (driveSlider);

    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "DRIVE", driveSlider);

    // Dimensioni finestra proporzionate all'immagine originale
    setSize (400, 560);
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
    // Centraggio dinamico calcolato sulle proporzioni del pannello in legno
    const int knobSize = static_cast<int>(getWidth() * 0.68f); // 68% della larghezza della GUI
    const int knobX = (getWidth() - knobSize) / 2;
    const int knobY = static_cast<int>(getHeight() * 0.44f);  // Posizionato esattamente al centro dell'arco graduato

    driveSlider.setBounds (knobX, knobY, knobSize, knobSize);
}