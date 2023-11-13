/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <complex>

//==============================================================================
RobotizerAudioProcessor::RobotizerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

RobotizerAudioProcessor::~RobotizerAudioProcessor()
{
}

//==============================================================================
const juce::String RobotizerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RobotizerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RobotizerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RobotizerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RobotizerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RobotizerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RobotizerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RobotizerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String RobotizerAudioProcessor::getProgramName (int index)
{
    return {};
}

void RobotizerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void RobotizerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    buffersize = &temp;
    setRateAndBufferSizeDetails(sampleRate, 512);
    createWavetable();
    updateDeltas();
    //temp = samplesPerBlock;
}

void RobotizerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RobotizerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void RobotizerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (turnedOn)
    {
        updateDeltas();
        juce::dsp::FFT k = juce::dsp::FFT::FFT(9);
        temp = buffer.getNumSamples();
        std::complex<float> inputarray[512];
        std::complex<float> outputarray[512];
        for (auto chan = 0; chan < 2; ++chan)
        {
            auto* channelData = buffer.getReadPointer(chan);
            float amplitude = 0.0;
            for (auto i = 0; i < temp; ++i)
            {
                inputarray[i] = { channelData[i],0 };
                if (std::abs(channelData[i]) > amplitude)
                {
                    amplitude = abs(channelData[i]);
                }
            }
            k.perform(inputarray, outputarray, false); // computing FFT of given array
            for (auto i = 0; i < temp; ++i) // zeroing components for negative frequencies
            {
                if (i == 0 || i == temp / 2)
                {

                }
                else if (i < temp / 2)
                {
                    outputarray[i] = { outputarray[i].real() * 2, outputarray[i].imag() * 2 };
                }
                else
                {
                    outputarray[i] = { 0,0 };
                }
            }
            std::complex<float> hilbertcomp[512];
            k.perform(outputarray, hilbertcomp, true); //reverse FFT/Hilbert
            float mh[512]; // Phase-shifted signal
            float normalizer = 0;
            for (auto i = 0; i < temp; ++i)
            {
                mh[i] = (float)hilbertcomp[i].imag();
                if (std::abs(mh[i] > normalizer))
                    normalizer = std::abs(mh[i]);
            }
            for (auto i = 0; i < temp; ++i)
            {
                mh[i] = mh[i] / normalizer * amplitude;
            }

            auto* data = buffer.getWritePointer(chan);
            auto* table = signalTable.getReadPointer(0);
            for (auto i = 0; i < temp; ++i)
            {
                auto sbuCosValue = getCosNextSample(0);
                auto sbuSinValue = getNextSample(0);
                auto sblCosValue = getCosNextSample(1);
                auto sblSinValue = getNextSample(1);
                float sbu = data[i] * sbuCosValue - mh[i] * sbuSinValue; //upper sideband
                float sbl = data[i] * sblCosValue + mh[i] * sblSinValue; //lower sideband

                data[i] = ((100 - wetdry) / 100) * (data[i]) + (wetdry / 100) * (signalAmp * data[i] + sbuAmp * sbu + sblAmp * sbl);
            }
        }
    }
}

//==============================================================================
bool RobotizerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* RobotizerAudioProcessor::createEditor()
{
    return new RobotizerAudioProcessorEditor (*this);
}

//==============================================================================
void RobotizerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void RobotizerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RobotizerAudioProcessor();
}
void RobotizerAudioProcessor::createWavetable()
{
    signalTable.setSize(1, tableSize);
    cosSignalTable.setSize(1, tableSize);
    auto* samples = signalTable.getWritePointer(0);
    auto* cossamples = cosSignalTable.getWritePointer(0);
    auto angleDelta = juce::MathConstants<double>::twoPi /(double)(tableSize - 1);
    auto currentAngle = 0.0;
    for (auto i = 0; i < tableSize; ++i)
    {
        auto sample = std::sin(currentAngle);
        auto cossample = std::cos(currentAngle);
        samples[i] = (float)sample;
        cossamples[i] = (float)cossample;
        currentAngle += angleDelta;
    }
}
forcedinline float RobotizerAudioProcessor::getNextSample(float option) noexcept
{
    auto currentSample = 0.0f;
    if (option == 0)//0-sbu,1-sbl 
    {
        auto tableSize = signalTable.getNumSamples();
        auto index0 = (unsigned int)sbuCurrentIndex;
        auto index1 = index0 == (tableSize - 1) ? (unsigned int)0 : index0 + 1;
        auto frac = sbuCurrentIndex - (float)index0;
        auto* table = signalTable.getReadPointer(0);
        auto value0 = table[index0];
        auto value1 = table[index1];
        currentSample = value0 + frac * (value1 - value0);
        if ((sbuCurrentIndex += sbuTableDelta) > tableSize)
            sbuCurrentIndex -= tableSize;
    }
    else if(option == 1)
    {
        auto tableSize = signalTable.getNumSamples();
        auto index0 = (unsigned int)sblCurrentIndex;
        auto index1 = index0 == (tableSize - 1) ? (unsigned int)0 : index0 + 1;
        auto frac = sblCurrentIndex - (float)index0;
        auto* table = signalTable.getReadPointer(0);
        auto value0 = table[index0];
        auto value1 = table[index1];
        currentSample = value0 + frac * (value1 - value0);
        if ((sblCurrentIndex += sblTableDelta) > tableSize)
            sblCurrentIndex -= tableSize;
    }
    return currentSample;
}

forcedinline float RobotizerAudioProcessor::getCosNextSample(float option) noexcept
{
    auto currentSample = 0.0f;
    if (option == 0)//0-sbu,1-sbl 
    {
        auto tableSize = cosSignalTable.getNumSamples();
        auto index0 = (unsigned int)sbuCosCurrentIndex;
        auto index1 = index0 == (tableSize - 1) ? (unsigned int)0 : index0 + 1;
        auto frac = sbuCosCurrentIndex - (float)index0;
        auto* table = cosSignalTable.getReadPointer(0);
        auto value0 = table[index0];
        auto value1 = table[index1];
        currentSample = value0 + frac * (value1 - value0);
        if ((sbuCosCurrentIndex += sbuCosTableDelta) > tableSize)
            sbuCosCurrentIndex -= tableSize;
    }
    else if (option == 1)
    {
        auto tableSize = cosSignalTable.getNumSamples();
        auto index0 = (unsigned int)sblCosCurrentIndex;
        auto index1 = index0 == (tableSize - 1) ? (unsigned int)0 : index0 + 1;
        auto frac = sblCosCurrentIndex - (float)index0;
        auto* table = cosSignalTable.getReadPointer(0);
        auto value0 = table[index0];
        auto value1 = table[index1];
        currentSample = value0 + frac * (value1 - value0);
        if ((sblCosCurrentIndex += sblCosTableDelta) > tableSize)
            sblCosCurrentIndex -= tableSize;
    }
    return currentSample;
}
void RobotizerAudioProcessor::updateDeltas()
{
    auto tableSizeOverSampleRate = tableSize / getSampleRate();
    sblTableDelta = sblSinFreq * tableSizeOverSampleRate;
    sbuTableDelta = sbuSinFreq * tableSizeOverSampleRate;
    sblCosTableDelta = sblCosFreq * tableSizeOverSampleRate;
    sbuCosTableDelta = sbuCosFreq * tableSizeOverSampleRate;
}