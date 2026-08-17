#include "PluginProcessor.h"
#include "PluginEditor.h"

OneKnobSaturatorAudioProcessorEditor::OneKnobSaturatorAudioProcessorEditor (OneKnobSaturatorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Caricamento dello sfondo in legno
    backgroundImage = juce::ImageCache::getFromMemory (BinaryData::background_oneknob_png, BinaryData::background_oneknob_pngSize);

    // Configurazione Manopola
    driveSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    driveSlider.setLookAndFeel (&customKnobLAF);
    
    // Angoli di rotazione allineati con i pallini 0 e MAX sulla grafica in legno
    driveSlider.setRotaryParameters (juce::MathConstants<float>::pi * 1.25f, 
                                     juce::MathConstants<float>::pi * 2.75f, true);
    
    addAndMakeVisible (driveSlider);

    // Attachment APVTS
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "DRIVE", driveSlider);

    // Proporzioni basate sull'immagine di sfondo
    setSize (450, 630);
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
        g.fillAll (juce::Colours::darkgrey);
    }
}

void OneKnobSaturatorAudioProcessorEditor::resized()
{
    // Posizionamento e ridimensionamento preciso della manopola al centro della scala incisa
    int knobSize = 260;
    int knobX = (getWidth() - knobSize) / 2;
    int knobY = 280; // Allineato al centro del cerchio graduato

    driveSlider.setBounds (knobX, knobY, knobSize, knobSize);
}