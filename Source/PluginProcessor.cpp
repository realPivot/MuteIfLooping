/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MuteIfLoopingAudioProcessor::MuteIfLoopingAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), treeState(*this, nullptr,"PARAMETERS", createParameterLayout())
#endif
{
    treeState.addParameterListener("looping", this);
}

MuteIfLoopingAudioProcessor::~MuteIfLoopingAudioProcessor()
{
    treeState.removeParameterListener("looping", this);
}

void MuteIfLoopingAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "looping")
    {
        pLoop = newValue;
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout MuteIfLoopingAudioProcessor::createParameterLayout()
{
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;

    auto pLoop = std::make_unique<juce::AudioParameterFloat>("looping", "Looping", 0.0, 1.0, 0.0);

    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String MuteIfLoopingAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MuteIfLoopingAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MuteIfLoopingAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MuteIfLoopingAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MuteIfLoopingAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MuteIfLoopingAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MuteIfLoopingAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MuteIfLoopingAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MuteIfLoopingAudioProcessor::getProgramName (int index)
{
    return {};
}

void MuteIfLoopingAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MuteIfLoopingAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    gain.reset(sampleRate, 0.05);
}

void MuteIfLoopingAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MuteIfLoopingAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MuteIfLoopingAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    auto playHead = getPlayHead();
    auto playHeadPosition = playHead->getPosition();
    if (playHeadPosition) {
        auto loopPoints = playHeadPosition->getLoopPoints();
        auto isLoopEnabledInDaw = playHeadPosition->getIsLooping();
        if (loopPoints.hasValue() && isLoopEnabledInDaw)
        {
            setIsLooping(true);
            gain.setTargetValue(0.0f);
        }
        else
        {
            setIsLooping(false);
            gain.setTargetValue(1.f); 
        }
    }


    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (auto sample = 0; sample < buffer.getNumSamples(); sample++)
        {
            channelData[sample] = channelData[sample] * gain.getNextValue();
        }
        
    }
}

//==============================================================================
bool MuteIfLoopingAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MuteIfLoopingAudioProcessor::createEditor()
{
    return new MuteIfLoopingAudioProcessorEditor (*this);
}

//==============================================================================
void MuteIfLoopingAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MuteIfLoopingAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void MuteIfLoopingAudioProcessor::setIsLooping(bool set) 
{
    isLooping = set;
}
bool MuteIfLoopingAudioProcessor::getIsLooping()
{
    return isLooping;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MuteIfLoopingAudioProcessor();
}
