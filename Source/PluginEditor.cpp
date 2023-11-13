/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RobotizerAudioProcessorEditor::RobotizerAudioProcessorEditor (RobotizerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 400);

    button.setButtonText("On");
    button.setToggleState(true, juce::sendNotification);
    addAndMakeVisible(&button);
    button.onClick = [this] {clickedButton(); };

    sliderAmpSbl.setRange(0, 1, 0.01);
    sliderAmpSbl.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    sliderAmpSbl.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sliderAmpSbl.setPopupDisplayEnabled(true, false, this);
    //sliderAmpSbl.setTextValueSuffix("Lower Sideband Amplitude");
    sliderAmpSbl.setValue(0.1);
    addAndMakeVisible(&sliderAmpSbl);
    sliderAmpSbl.addListener(this);

    sliderAmpSbu.setRange(0, 1, 0.01);
    sliderAmpSbu.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    sliderAmpSbu.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sliderAmpSbu.setPopupDisplayEnabled(true, false, this);
    //sliderAmpSbu.setTextValueSuffix("Upper Sideband Amplitude");
    sliderAmpSbu.setValue(0.1);
    addAndMakeVisible(&sliderAmpSbu);
    sliderAmpSbu.addListener(this);

    sliderAmpSignal.setRange(0, 1, 0.01);
    sliderAmpSignal.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    sliderAmpSignal.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sliderAmpSignal.setPopupDisplayEnabled(true, false, this);
    //sliderAmpSignal.setTextValueSuffix("Source Signal Amplitude");
    sliderAmpSignal.setValue(0.8);
    addAndMakeVisible(&sliderAmpSignal);
    sliderAmpSignal.addListener(this);

    sliderSblCosFreq.setRange(0, 500, 1);
    sliderSblCosFreq.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    sliderSblCosFreq.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sliderSblCosFreq.setPopupDisplayEnabled(true, false, this);
    sliderSblCosFreq.setTextValueSuffix("Hz");
    sliderSblCosFreq.setValue(300);
    addAndMakeVisible(&sliderSblCosFreq);
    sliderSblCosFreq.addListener(this);

    sliderSblSinFreq.setRange(0, 500, 1);
    sliderSblSinFreq.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    sliderSblSinFreq.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sliderSblSinFreq.setPopupDisplayEnabled(true, false, this);
    sliderSblSinFreq.setTextValueSuffix("Hz");
    sliderSblSinFreq.setValue(300);
    addAndMakeVisible(&sliderSblSinFreq);
    sliderSblSinFreq.addListener(this);

    sliderSbuCosFreq.setRange(0, 500, 1);
    sliderSbuCosFreq.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    sliderSbuCosFreq.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sliderSbuCosFreq.setPopupDisplayEnabled(true, false, this);
    sliderSbuCosFreq.setTextValueSuffix("Hz");
    sliderSbuCosFreq.setValue(300);
    addAndMakeVisible(&sliderSbuCosFreq);
    sliderSbuCosFreq.addListener(this);

    sliderSbuSinFreq.setRange(0, 500, 1);
    sliderSbuSinFreq.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    sliderSbuSinFreq.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sliderSbuSinFreq.setPopupDisplayEnabled(true, false, this);
    sliderSbuSinFreq.setTextValueSuffix("Hz");
    sliderSbuSinFreq.setValue(300);
    addAndMakeVisible(&sliderSbuSinFreq);
    sliderSbuSinFreq.addListener(this);

    sliderWetDry.setRange(0, 100, 1);
    sliderWetDry.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    sliderWetDry.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sliderWetDry.setPopupDisplayEnabled(true, false, this);
    sliderWetDry.setTextValueSuffix("% Wet");
    sliderWetDry.setValue(100);
    addAndMakeVisible(&sliderWetDry);
    sliderWetDry.addListener(this);

    labelAmpSbl.setText("Lower Sideband Amplitude", juce::NotificationType::dontSendNotification);
    labelAmpSbl.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&labelAmpSbl);
    labelAmpSbu.setText("Upper Sideband Amplitude", juce::NotificationType::dontSendNotification);
    labelAmpSbu.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&labelAmpSbu);
    labelAmpSignal.setText("Signal Amplitude", juce::NotificationType::dontSendNotification);
    labelAmpSignal.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&labelAmpSignal);

    labelSblCosFreq.setText("Lower Sideband Cos Freq", juce::NotificationType::dontSendNotification);
    labelSblCosFreq.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&labelSblCosFreq);
    labelSbuCosFreq.setText("Upper Sideband Cos Freq", juce::NotificationType::dontSendNotification);
    labelSbuCosFreq.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&labelSbuCosFreq);
    labelSblSinFreq.setText("Lower Sideband Sin Freq", juce::NotificationType::dontSendNotification);
    labelSblSinFreq.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&labelSblSinFreq);
    labelSbuSinFreq.setText("Upper Sideband Sin Freq", juce::NotificationType::dontSendNotification);
    labelSbuSinFreq.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&labelSbuSinFreq);

    labelWetDry.setText("Dry/Wet:", juce::NotificationType::dontSendNotification);
    labelWetDry.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&labelWetDry);

}

RobotizerAudioProcessorEditor::~RobotizerAudioProcessorEditor()
{
}

//==============================================================================
void RobotizerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void RobotizerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    button.setBounds(25, 335, 50, 50);
    sliderAmpSbl.setBounds(50, 60, 100, 100);
    sliderAmpSbu.setBounds(200, 60, 100, 100);
    sliderAmpSignal.setBounds(350, 60, 100, 100);
    sliderSblCosFreq.setBounds(20, 220, 100, 100);
    sliderSbuCosFreq.setBounds(140, 220, 100, 100);
    sliderSblSinFreq.setBounds(260, 220, 100, 100);
    sliderSbuSinFreq.setBounds(380, 220, 100, 100);
    sliderWetDry.setBounds(435, 335, 50, 50);

    labelAmpSbl.setBounds(50, 25, 100, 40);
    labelAmpSbu.setBounds(200, 25, 100, 40);
    labelAmpSignal.setBounds(350, 20, 100, 40);

    labelSblCosFreq.setBounds(20, 185, 100, 40);
    labelSbuCosFreq.setBounds(140, 185, 100, 40);
    labelSblSinFreq.setBounds(260, 185, 100, 40);
    labelSbuSinFreq.setBounds(380, 185, 100, 40);

    labelWetDry.setBounds(435, 300, 50, 40);

}

void RobotizerAudioProcessorEditor::clickedButton()
{
    auto state = button.getToggleState();
    audioProcessor.turnedOn = (bool)state;

}

void RobotizerAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    float sr = (float)audioProcessor.getSampleRate();
    audioProcessor.sblAmp = sliderAmpSbl.getValue();
    audioProcessor.sbuAmp = sliderAmpSbu.getValue();
    audioProcessor.signalAmp = sliderAmpSignal.getValue();
    audioProcessor.sblCosFreq = sliderSblCosFreq.getValue();
    audioProcessor.sblSinFreq = sliderSblSinFreq.getValue();
    audioProcessor.sbuCosFreq = sliderSbuCosFreq.getValue();
    audioProcessor.sbuSinFreq = sliderSbuSinFreq.getValue();
    audioProcessor.wetdry = sliderWetDry.getValue();
}