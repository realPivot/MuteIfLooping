/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MuteIfLoopingAudioProcessorEditor::MuteIfLoopingAudioProcessorEditor (MuteIfLoopingAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (150 , 85);
    startTimerHz(24);

    
    addAndMakeVisible(mainLabel);
    mainLabel.setText("initialized", juce::NotificationType::dontSendNotification);
    mainLabel.setEnabled(true);
    mainLabel.setFont(15.0f);
    mainLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    mainLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentWhite);
    mainLabel.setEditable(false, false, false);

    addAndMakeVisible(versionLabel);
    versionLabel.setText(JucePlugin_VersionString, juce::dontSendNotification);
    versionLabel.setFont(12.f);
    mainLabel.copyAllExplicitColoursTo(versionLabel);
    versionLabel.setEditable(false, false, false);
}

MuteIfLoopingAudioProcessorEditor::~MuteIfLoopingAudioProcessorEditor()
{
}

//==============================================================================
void MuteIfLoopingAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (backgroundColour);

    //g.setColour (juce::Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void MuteIfLoopingAudioProcessorEditor::timerCallback()
{
    isLooping = audioProcessor.getIsLooping();

    if (isLooping)
    {
        enableLoop();
    }
    else
    {
        disableLoop();
    }
}

void MuteIfLoopingAudioProcessorEditor::resized()
{
    auto bounds = getBounds();
    mainLabel.setBounds(bounds);
    mainLabel.setJustificationType(juce::Justification::centred);

    versionLabel.setBounds(bounds);
    versionLabel.setJustificationType(juce::Justification::bottomRight);
}

void MuteIfLoopingAudioProcessorEditor::enableLoop()
{
    backgroundColour = juce::Colour(135,61,72); // Cordovan red
    mainLabel.setText("Looping", juce::NotificationType::dontSendNotification);
    mainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
}

void MuteIfLoopingAudioProcessorEditor::disableLoop()
{
    backgroundColour = juce::Colour(190,255,199); // tea green
    mainLabel.setText("Not Looping", juce::NotificationType::dontSendNotification);
    mainLabel.setColour(juce::Label::textColourId, juce::Colours::black);
}
