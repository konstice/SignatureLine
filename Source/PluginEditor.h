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
class KennlinieAudioProcessorEditor : public juce::AudioProcessorEditor,
    public juce::Timer,
    public juce::ChangeListener,
    private juce::Slider::Listener
{
public:
    KennlinieAudioProcessorEditor (KennlinieAudioProcessor&);
    ~KennlinieAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void changeListenerCallback(juce::ChangeBroadcaster* sender) override;
    void timerCallback() override;

    //letzter zustand; false=pos; true=neg;
    bool xsync = false, ysync = false;

private:

    void sliderValueChanged(juce::Slider* slider) override;
    void setSliderStyle(juce::Slider* slider);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    KennlinieAudioProcessor& audioProcessor;



    //in und outgain slider; einzelne gruppen, sollen später noch verändert werden
    juce::Slider ingainSlider, outgainSlider;

    //slider für die kennlinie
    juce::Slider xposSlider, xnegSlider, yposSlider, ynegSlider;

    juce::Rectangle<int> plotFrame;

    juce::HyperlinkButton copyrightButton;

    juce::ComboBox kurvename;

    juce::ToggleButton sync_x, sync_y;

    juce::ComboBox filterType;
    juce::Slider freqcSlider;
    juce::Slider qSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KennlinieAudioProcessorEditor)
};
