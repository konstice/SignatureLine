/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "graph.h"
#include "biquad.h"

//==============================================================================
/**
*/
class KennlinieAudioProcessor  : public juce::AudioProcessor,
    public juce::ChangeBroadcaster
{
public:

    float ingain_value=0, outgain_value=0;

    float xpos_value = 0.5, ypos_value = 0;
    float xneg_value = -0.5, yneg_value = -0.5;

    float buffer_input[64], buffer_output[64];

    int buffer_stelle;

    bool x_sync=false, y_sync=false;

    int art = 2;

    int filterType=16;

    float freqc=100;
    float q_value=0.707;

    //==============================================================================
    KennlinieAudioProcessor();
    ~KennlinieAudioProcessor() override;

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
    juce::Point<int> getSavedSize() const;
    void setSavedSize(const juce::Point<int>& size);

    float processingKurve(float input);

    float db2float(float input) {
        return pow(10, input / 20);
    }

    float float2db(float input) {
        return 20 * log10(input);
    }

    BiQuad filterL, filterR;
private:

    float lagrange(double* x, double* y, int n, float xbar);
    
    void setFilter(BiQuad input);

    juce::Point<int> editorSize = { 600, 500 };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KennlinieAudioProcessor)
};
