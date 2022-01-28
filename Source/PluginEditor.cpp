/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
KennlinieAudioProcessorEditor::KennlinieAudioProcessorEditor (KennlinieAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    kurvename.addItem("Bypass", CT_Bypass);
    kurvename.addItem("ArcTan", CT_ArcTan);
    kurvename.addItem("ArcTan 2", CT_ArcTan2);
    kurvename.addItem("Strange", CT_Strange);
    kurvename.addItem("La Grange", CT_Lagrange);
    kurvename.addItem("Gaus", CT_Gaus);
    kurvename.setSelectedId(CT_Bypass);
    addAndMakeVisible(&kurvename);
    kurvename.onChange = [this] {audioProcessor.art = kurvename.getSelectedId();};
    //kurvename.addListener(this);

    filterType.addItem("Bypass", GenericBiquad::FT_ByPass);
    filterType.addItem("LPF", GenericBiquad::FT_LowPass);
    filterType.addItem("HPF", GenericBiquad::FT_HighPass);
    filterType.addItem("BPF", GenericBiquad::FT_BandPass);
    filterType.addItem("BPS", GenericBiquad::FT_BandStop);
    filterType.setSelectedId(GenericBiquad::FT_ByPass);
    addAndMakeVisible(&filterType);
    filterType.onChange = [this] {audioProcessor.filterType = static_cast<GenericBiquad::FilterType>(filterType.getSelectedId());};

    addAndMakeVisible(sync_x);
    sync_x.onClick = [this]
    {
        audioProcessor.x_sync = sync_x.getToggleState();
        if (xsync == false)
        {
            xnegSlider.setValue(-xposSlider.getValue());
        }
        else
        {
            xposSlider.setValue(-xnegSlider.getValue());
        }
    };

    addAndMakeVisible(sync_y);
    sync_y.onClick = [this] 
    {
        audioProcessor.y_sync = sync_y.getToggleState();
        if (ysync == false)
        {
            ynegSlider.setValue(-yposSlider.getValue());
        }
        else
        {
            yposSlider.setValue(-ynegSlider.getValue());
        }
    };

    //freqc slider
    freqcSlider.setSliderStyle(juce::Slider::Rotary);
    //freqcSlider.setBounds(getWidth() - 100, 50, 75, getHeight() - 25 - 50);
    freqcSlider.setRange(20, 20000, 0.1);
    freqcSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 15);
    freqcSlider.setSkewFactorFromMidPoint(5000);
    freqcSlider.setPopupDisplayEnabled(true, false, this);
    freqcSlider.setValue(100);
    freqcSlider.setTextValueSuffix("Hz");
    addAndMakeVisible(&freqcSlider);
    freqcSlider.addListener(this);

    //q slider
    qSlider.setSliderStyle(juce::Slider::Rotary);
    //qSlider.setBounds(getWidth() - 100, 50, 75, getHeight() - 25 - 50);
    qSlider.setRange(0.5, 20, 0.1);
    qSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 15);
    qSlider.setPopupDisplayEnabled(true, false, this);
    qSlider.setSkewFactorFromMidPoint(3);
    qSlider.setValue(0.707);
    addAndMakeVisible(&qSlider);
    qSlider.addListener(this);

    //kennlinie slider
    xposSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    //xposSlider.setBounds(getWidth() - 100, 50, 75, getHeight() - 25 - 50);
    xposSlider.setRange(0, 1, 0.01);
    xposSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 25);
    xposSlider.setPopupDisplayEnabled(false, false, this);
    xposSlider.setValue(0.5);
    addAndMakeVisible(&xposSlider);
    xposSlider.addListener(this);

    yposSlider.setSliderStyle(juce::Slider::LinearVertical);
    //yposSlider.setBounds(getWidth() - 100, 50, 75, getHeight() - 25 - 50);
    yposSlider.setRange(0, 1, 0.01);
    yposSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 75, 25);
    yposSlider.setValue(0.5);
    addAndMakeVisible(&yposSlider);
    yposSlider.addListener(this);

    xnegSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    //xnegSlider.setBounds(getWidth() - 100, 50, 75, getHeight() - 25 - 50);
    xnegSlider.setRange(-1, 0, 0.01);
    xnegSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 75, 25);
    xnegSlider.setValue(-0.5);
    addAndMakeVisible(&xnegSlider);
    xnegSlider.addListener(this);

    ynegSlider.setSliderStyle(juce::Slider::LinearVertical);
    //ynegSlider.setBounds(getWidth() - 100, 50, 75, getHeight() - 25 - 50);
    ynegSlider.setRange(-1,0,0.01);
    ynegSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 75, 25);
    ynegSlider.setValue(-0.5);
    addAndMakeVisible(&ynegSlider);
    ynegSlider.addListener(this);

    //in und outgain slider
    outgainSlider.setSliderStyle(juce::Slider::LinearVertical);
    outgainSlider.setBounds(getWidth() - 100, 50, 75, getHeight() - 25 - 50);
    outgainSlider.setRange(-18.00, 6.00, 0.1);
    outgainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 75, 25);
    outgainSlider.setPopupDisplayEnabled(true, false, this);
    outgainSlider.setTextValueSuffix("dB");
    outgainSlider.setValue(0.00);
    addAndMakeVisible(&outgainSlider);
    outgainSlider.addListener(this);

    setSliderStyle(&ingainSlider);
    addAndMakeVisible(&ingainSlider);
    ingainSlider.addListener(this);

    auto size = audioProcessor.getSavedSize();
    setSize(size.x, size.y);
    /*setResizable(false, false);
    setSize(size.x, size.y);
    setResizeLimits(300, 250, 1200, 1000);*/

    audioProcessor.addChangeListener (this);

    startTimerHz(30);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    //setSize (600, 500);
}

void KennlinieAudioProcessorEditor::setSliderStyle(juce::Slider* slider)
{
    slider->setSliderStyle(juce::Slider::LinearVertical);
    slider->setBounds(25, 75, 75, getHeight() - 25 - 50);
    slider->setRange(-18, 6, 0.1);
    slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 75, 25);
    slider->setPopupDisplayEnabled(true, false, this);
    slider->setTextValueSuffix("dB");
    slider->setValue(0);
}

KennlinieAudioProcessorEditor::~KennlinieAudioProcessorEditor()
{
    audioProcessor.removeChangeListener (this);
}

void KennlinieAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* sender)
{
    /*ignoreUnused(sender);
    updateFrequencyResponses();
    repaint();*/
}

void KennlinieAudioProcessorEditor::timerCallback()
{
    repaint(plotFrame);
}


void KennlinieAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &ingainSlider)
    {
        audioProcessor.ingain_value = ingainSlider.getValue();
    }
    if (slider == &outgainSlider)
    {
        audioProcessor.outgain_value = outgainSlider.getValue();
    }
    if (slider == &xposSlider)
    {
        if (sync_x.getToggleState() == true)
        {
            audioProcessor.xneg_value = -xposSlider.getValue();
            xnegSlider.setValue(-xposSlider.getValue());
            
        }
        audioProcessor.xpos_value = xposSlider.getValue();
        xsync = false;
    }
    if (slider == &yposSlider)
    {
        if (sync_y.getToggleState() == true)
        {
            audioProcessor.yneg_value = -yposSlider.getValue();
            ynegSlider.setValue(-yposSlider.getValue());
        }
        audioProcessor.ypos_value = yposSlider.getValue();
        ysync = false;
    }
    if (slider == &xnegSlider)
    {
        if (sync_x.getToggleState() == true)
        {
            audioProcessor.xneg_value = -xposSlider.getValue();
            xposSlider.setValue(-xnegSlider.getValue());
        }
        else
        {
            audioProcessor.xneg_value = xnegSlider.getValue();
        }
        xsync = true;
    }
    if (slider == &ynegSlider)
    {
        if (sync_y.getToggleState() == true)
        {
            audioProcessor.yneg_value = -yposSlider.getValue();
            yposSlider.setValue(-ynegSlider.getValue());
        }
        else
        {
            audioProcessor.yneg_value = ynegSlider.getValue();
        }
        ysync = true;
    }
    if (slider == &freqcSlider)
    {
        audioProcessor.freqc = freqcSlider.getValue();
    }
    if (slider == &qSlider)
    {
        audioProcessor.q_value = qSlider.getValue();
    }

}

//==============================================================================
void KennlinieAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::Graphics::ScopedSaveState state (g);

    plotFrame = { 200, 175, 200, 200 };


    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::tomato);
    
    
    g.setColour(juce::Colours::black);

    //ingain
    g.drawRect(25, 25, 75, getHeight() - 25 - 25);
    //out gain
    g.drawRect(getWidth() - 100, 25, 75, getHeight() - 25 - 25);
    //eq section
    g.drawRect(125, 25, 350, 100);
    //distortion
    g.drawRect(125, 150, 350, getHeight()-150-25);

    /*
    //box für die kennlinie
    g.drawRect(200, 175, 200, 200);
    
    
    */
    //kennlinieshizzle
    
    g.drawRect(200, 275, 200, 1);
    g.drawRect(300, 175, 1, 200);
    //kennlinienen shizzle
    //g.drawRect(plotFrame,5);
    //g.drawRoundedRectangle(plotFrame.toFloat(),5,2);
    g.setColour(juce::Colours::silver);
    float y_value = 0.001;
    y_value = audioProcessor.processingKurve(y_value);
    y_value *= 100;
    float y_value_neg = -0.001;
    y_value_neg = 100 * audioProcessor.processingKurve(y_value_neg);
    g.drawLine(300, 275 - y_value - 1, 300, 275 - y_value_neg + 1, 4);
    for (int k = 1;k < 100;k++)
    {
        //y_value = (int )(100 * audioProcessor.processingKurve(k / 100));
        //g.drawEllipse(300 + k-1, 275 - y_value-1, 2, 2, 2);
        y_value =(float) k / 100;
        y_value = audioProcessor.processingKurve(y_value);
        y_value *= 100;
        g.drawEllipse(300 + k - 1,275-y_value-1, 2, 2, 2);
        y_value = -(float)k / 100;
        y_value = audioProcessor.processingKurve(y_value);
        y_value *= 100;
        g.drawEllipse(300 - k - 1, 275 - y_value - 1, 2, 2, 2);
    }

    g.setColour(juce::Colours::cyan);
    g.drawEllipse(300 + 100 * audioProcessor.xpos_value-1, 275 - 100 * audioProcessor.ypos_value-1,2,2,5);
    g.drawEllipse(300 + 100 * audioProcessor.xneg_value - 1, 275 - 100 * audioProcessor.yneg_value - 1, 2, 2, 5);

    g.setColour(juce::Colours::silver);
    g.drawRoundedRectangle(plotFrame.toFloat(), 5, 2);

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Kennlinie V1", getLocalBounds(), juce::Justification::centredTop, 1);

    g.drawFittedText("Input Gain", 25, 25, 75, 25,juce::Justification::centred, 2);
    g.drawFittedText("Output Gain", getWidth() - 100, 25, 75, 25, juce::Justification::centred, 2);
    g.drawFittedText("Kennlinie", 200, 150, 200, 35, juce::Justification::centred, 1);

    
}

void KennlinieAudioProcessorEditor::resized()
{

    ingainSlider.setBounds(25, 50, 75, getHeight() - 25 - 50);
    outgainSlider.setBounds(getWidth() - 100, 50, 75, getHeight() - 25 - 50);

    xposSlider.setBounds(300+5, 375, 100, 25);
    yposSlider.setBounds(175, 175, 25, 100-5);
    xnegSlider.setBounds(200, 375, 100-5, 25);
    ynegSlider.setBounds(175, 275+5, 25, 100);

    kurvename.setBounds(200, 400, 200, 25);

    filterType.setBounds(200, 35, 200, 25);

    freqcSlider.setBounds(200, 60, 100, 65);
    qSlider.setBounds(300, 60, 100, 65);

    sync_y.setBounds(175, 262, 25, 25);
    sync_x.setBounds(287, 375, 25, 25);

    //copyrightButton.setBounds(getWidth() - 200, getHeight() - 25, 200, 25);

    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}  
