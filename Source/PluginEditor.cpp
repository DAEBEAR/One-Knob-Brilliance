#include "PluginProcessor.h"
#include "PluginEditor.h"

OneKnobSaturatorAudioProcessorEditor::OneKnobSaturatorAudioProcessorEditor (OneKnobSaturatorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Brand Subtitle
    brandLabel.setText("DAEBAER PLUGINS", juce::dontSendNotification);
    brandLabel.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    brandLabel.setJustificationType(juce::Justification::centred);
    brandLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(brandLabel);

    // Titolo Plugin
    titleLabel.setText("ONE KNOB SATURATOR", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(16.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
    addAndMakeVisible(titleLabel);

    // Drive Slider
    driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(driveSlider);

    driveLabel.setText("DRIVE", juce::dontSendNotification);
    driveLabel.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    driveLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(driveLabel);

    // Mode Selector
    modeSelector.addItem("Warm Tape", 1);
    modeSelector.addItem("Tube Saturation", 2);
    modeSelector.addItem("Diode Clipper", 3);
    addAndMakeVisible(modeSelector);

    modeLabel.setText("MODE", juce::dontSendNotification);
    modeLabel.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    modeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(modeLabel);

    // Attachments APVTS
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "DRIVE", driveSlider);
    
    modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.apvts, "MODE", modeSelector);

    setSize (300, 420);
}

OneKnobSaturatorAudioProcessorEditor::~OneKnobSaturatorAudioProcessorEditor()
{
}

void OneKnobSaturatorAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff141414));

    g.setColour (juce::Colour (0xff2d2d2d));
    g.drawRoundedRectangle (10.0f, 10.0f, static_cast<float>(getWidth() - 20), static_cast<float>(getHeight() - 20), 10.0f, 2.0f);
}

void OneKnobSaturatorAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);

    brandLabel.setBounds(area.removeFromTop(16));
    titleLabel.setBounds(area.removeFromTop(24));
    area.removeFromTop(10);

    modeLabel.setBounds(area.removeFromTop(15));
    modeSelector.setBounds(area.removeFromTop(30).reduced(20, 0));

    area.removeFromTop(15);

    driveLabel.setBounds(area.removeFromTop(15));
    driveSlider.setBounds(area.removeFromTop(200));
}