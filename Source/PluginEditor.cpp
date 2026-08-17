#include "PluginProcessor.h"
#include "PluginEditor.h"

OneKnobSaturatorAudioProcessorEditor::OneKnobSaturatorAudioProcessorEditor (OneKnobSaturatorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Brand
    brandLabel.setText("DAEBAER PLUGINS", juce::dontSendNotification);
    brandLabel.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    brandLabel.setJustificationType(juce::Justification::centred);
    brandLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(brandLabel);

    // Title
    titleLabel.setText("ONE KNOB SATURATOR", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(16.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
    addAndMakeVisible(titleLabel);

    // Subtitle
    subtitleLabel.setText("WARM TAPE • 90Hz HPF", juce::dontSendNotification);
    subtitleLabel.setFont(juce::FontOptions(10.0f, juce::Font::plain));
    subtitleLabel.setJustificationType(juce::Justification::centred);
    subtitleLabel.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
    addAndMakeVisible(subtitleLabel);

    // Drive Knob
    driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(driveSlider);

    driveLabel.setText("DRIVE", juce::dontSendNotification);
    driveLabel.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    driveLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(driveLabel);

    // Attachment
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "DRIVE", driveSlider);

    setSize (280, 360);
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
    titleLabel.setBounds(area.removeFromTop(22));
    subtitleLabel.setBounds(area.removeFromTop(16));
    
    area.removeFromTop(20);

    driveLabel.setBounds(area.removeFromTop(15));
    driveSlider.setBounds(area.removeFromTop(200));
}