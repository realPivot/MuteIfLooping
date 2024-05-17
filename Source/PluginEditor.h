/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class MuteIfLoopingAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    MuteIfLoopingAudioProcessorEditor (MuteIfLoopingAudioProcessor&);
    ~MuteIfLoopingAudioProcessorEditor() override;

    //==============================================================================
    void timerCallback() override;
    void paint (juce::Graphics&) override;
    void resized() override;

    void enableLoop();
    void disableLoop();

    juce::Colour backgroundColour;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MuteIfLoopingAudioProcessor& audioProcessor;
    bool isLooping;
    juce::Label mainLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MuteIfLoopingAudioProcessorEditor)
};
