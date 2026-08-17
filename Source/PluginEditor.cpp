#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OneKnobSaturatorAudioProcessorEditor::OneKnobSaturatorAudioProcessorEditor (OneKnobSaturatorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // 1. Caricamento dell'immagine di sfondo PNG dal BinaryData
    backgroundImage = juce::ImageCache::getFromMemory (BinaryData::background_oneknob_png, 
                                                       BinaryData::background_oneknob_pngSize);

    // 2. Configurazione dello Slider
    driveSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    driveSlider.setLookAndFeel (&customKnobLAF);
    
    // Disabilita l'opacità per permettere al canale alfa (trasparenza) della manopola di mostrare il legno sottostante
    driveSlider.setOpaque (false);
    
    // Angoli di rotazione espressi in radianti, sincronizzati con le tacche '0' e 'MAX' della grafica
    driveSlider.setRotaryParameters (juce::MathConstants<float>::pi * 1.25f, 
                                     juce::MathConstants<float>::pi * 2.75f, 
                                     true);
    
    addAndMakeVisible (driveSlider);

    // 3. Collegamento dello Slider al parametro APVTS "DRIVE"
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "DRIVE", driveSlider);

    // Dimensioni della finestra (proporzionate alla grafica in legno)
    setSize (420, 588);
}

OneKnobSaturatorAudioProcessorEditor::~OneKnobSaturatorAudioProcessorEditor()
{
    // Ripristina il LookAndFeel predefinito prima della distruzione della classe
    driveSlider.setLookAndFeel (nullptr);
}

//==============================================================================
void OneKnobSaturatorAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Disegna l'immagine di sfondo in legno occupando l'intero rettangolo della GUI
    if (backgroundImage.isValid())
    {
        g.drawImage (backgroundImage, 
                     getLocalBounds().toFloat(), 
                     juce::RectanglePlacement::stretchToFit);
    }
    else
    {
        g.fillAll (juce::Colours::black);
    }
}

void OneKnobSaturatorAudioProcessorEditor::resized()
{
    // Calcolo preciso per centrare la manopola sull'arco graduato tra "0" e "MAX"
    const int knobSize = static_cast<int>(getWidth() * 0.54f);
    const int knobX = (getWidth() - knobSize) / 2;
    const int knobY = static_cast<int>(getHeight() * 0.50f);

    driveSlider.setBounds (knobX, knobY, knobSize, knobSize);
}