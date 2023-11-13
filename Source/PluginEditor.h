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
class RobotizerAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Slider::Listener
{
public:
    RobotizerAudioProcessorEditor (RobotizerAudioProcessor&);
    ~RobotizerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    void clickedButton();
    RobotizerAudioProcessor& audioProcessor;
    juce::ToggleButton button;
    juce::Slider sliderWetDry;
    juce::Slider sliderAmpSignal;
    juce::Slider sliderAmpSbu;
    juce::Slider sliderAmpSbl;
    juce::Slider sliderSbuCosFreq;
    juce::Slider sliderSbuSinFreq;
    juce::Slider sliderSblCosFreq;
    juce::Slider sliderSblSinFreq;

    juce::Label labelSbuCosFreq;
    juce::Label labelSbuSinFreq;
    juce::Label labelSblCosFreq;
    juce::Label labelSblSinFreq;
    juce::Label labelAmpSignal;
    juce::Label labelAmpSbu;
    juce::Label labelAmpSbl;
    juce::Label labelWetDry;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RobotizerAudioProcessorEditor)
};
