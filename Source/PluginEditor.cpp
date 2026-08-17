#include "PluginProcessor.h"
#include "PluginEditor.h"

OneKnobSaturatorAudioProcessorEditor::OneKnobSaturatorAudioProcessorEditor (OneKnobSaturatorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Titolo
    titleLabel.setText("ONE KNOB SATURATOR", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(18.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
    addAndMakeVisible(titleLabel);

    // Drive Slider (Manopolone principale)
    driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(driveSlider);

    driveLabel.setText("DRIVE", juce::dontSendNotification);
    driveLabel.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    driveLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(driveLabel);

    // Mode ComboBox (Interruttore Saturation Mode)
    modeSelector.addItem("Warm Tape", 1);
    modeSelector.addItem("Tube Saturation", 2);
    modeSelector.addItem("Diode Clipper", 3);
    addAndMakeVisible(modeSelector);

    modeLabel.setText("MODE", juce::dontSendNotification);
    modeLabel.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    modeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(modeLabel);

    // APVTS Attachments
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "DRIVE", driveSlider);
    
    modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.apvts, "MODE", modeSelector);

    setSize (300, 400);
}

OneKnobSaturatorAudioProcessorEditor::~OneKnobSaturatorAudioProcessorEditor()
{
}

void OneKnobSaturatorAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Sfondo dark elegant
    g.fillAll (juce::Colour (0xff1a1a1a));

    // Bordo interno stilizzato
    g.setColour (juce::Colour (0xff2d2d2d));
    g.drawRoundedRectangle (10.0f, 10.0f, static_cast<float>(getWidth() - 20), static_cast<float>(getHeight() - 20), 10.0f, 2.0f);
}

void OneKnobSaturatorAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);

    titleLabel.setBounds(area.removeFromTop(30));
    area.removeFromTop(10);

    // Mode Switcher in alto
    modeLabel.setBounds(area.removeFromTop(15));
    modeSelector.setBounds(area.removeFromTop(30).reduced(20, 0));

    area.removeFromTop(20);

    // Large Rotary Knob al centro
    driveLabel.setBounds(area.removeFromTop(15));
    driveSlider.setBounds(area.removeFromTop(200));
}