/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class RobotizerAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    RobotizerAudioProcessor();
    ~RobotizerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    float* buffersize;
    float temp = 0;
    float sbuCosFreq = 500;
    float sbuSinFreq = 500;
    float sblCosFreq = 500;
    float sblSinFreq = 500;
    float sblAmp = 0.1;
    float sbuAmp = 0.1;
    float signalAmp = 0.8;
    float wetdry = 100;
    bool turnedOn = true;
    void RobotizerAudioProcessor::createWavetable();
    void RobotizerAudioProcessor::updateDeltas();
private:
    float sblCurrentIndex = 0.0f;
    float sbuCurrentIndex = 0.0f;
    float sblTableDelta = 0.0f;
    float sbuTableDelta = 0.0f;
    float sblCosCurrentIndex = 0.0f;
    float sbuCosCurrentIndex = 0.0f;
    float sblCosTableDelta = 0.0f;
    float sbuCosTableDelta = 0.0f;
    float tableSize = 65536;

    forcedinline float RobotizerAudioProcessor::getNextSample(float option) noexcept;
    forcedinline float RobotizerAudioProcessor::getCosNextSample(float option) noexcept;
    
    juce::AudioSampleBuffer signalTable;
    juce::AudioSampleBuffer cosSignalTable;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RobotizerAudioProcessor)

};
