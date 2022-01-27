/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
KennlinieAudioProcessor::KennlinieAudioProcessor()
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
    filterL.init(getSampleRate(), filterType, freqc, q_value);
    filterR.init(getSampleRate(), filterType, freqc, q_value);
}

KennlinieAudioProcessor::~KennlinieAudioProcessor()
{

}

//==============================================================================
const juce::String KennlinieAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool KennlinieAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool KennlinieAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool KennlinieAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double KennlinieAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int KennlinieAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int KennlinieAudioProcessor::getCurrentProgram()
{
    return 0;
}

void KennlinieAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String KennlinieAudioProcessor::getProgramName (int index)
{
    return {};
}

void KennlinieAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void KennlinieAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    buffer_stelle = 0;

    filterL.init(getSampleRate(), filterType, freqc, q_value);
    filterR.init(getSampleRate(), filterType, freqc, q_value);
}

void KennlinieAudioProcessor::releaseResources()
{
    buffer_stelle = 0;
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    filterL.reset();
    filterR.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool KennlinieAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void KennlinieAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.


    for (int numSamples = 0; numSamples < buffer.getNumSamples(); numSamples++)
    {
        //buffer_input[buffer_stelle] = (buffer.getSample(0, numSamples) + buffer.getSample(1, numSamples)) / 2;

        for (int channel = 0;channel < buffer.getNumChannels();channel++)
        {
            auto* puffer = buffer.getWritePointer(channel, numSamples);

            *puffer *= db2float(ingain_value);

            if (channel == 0)
            {
                filterL.init(getSampleRate(), filterType, freqc, q_value);
                *puffer = filterL.process(*puffer);
            }
            else if (channel == 1)
            {
                filterR.init(getSampleRate(), filterType, freqc, q_value);
                *puffer = filterR.process(*puffer);
            }

            *puffer = processingKurve(*puffer);

            *puffer *= db2float(outgain_value);
        }

        //buffer_output[buffer_stelle] = (buffer.getSample(0, numSamples) + buffer.getSample(1, numSamples)) / 2;
    }



    /*for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int numSamples = 0; numSamples < buffer.getNumSamples(); numSamples++)
        {
            buffer_input[buffer_stelle] = (buffer.getSample(0, numSamples) + buffer.getSample(1, numSamples)) / 2;
            //auto sample = channelData[numSamples];
            channelData[numSamples] *= db2float(ingain_value);
            if (channel == 0)
            {
                filterL.init(getSampleRate(), filterType, freqc, q_value);
                
                //filterL.freqc = freqc;
                channelData[numSamples]=filterL.process(channelData[numSamples]);
            }
            else if (channel == 1)
            {
                filterR.init(getSampleRate(), filterType, freqc, q_value);
                channelData[numSamples]=filterR.process(channelData[numSamples]);
                
            }
            channelData[numSamples] = processingKurve(channelData[numSamples]);
            channelData[numSamples] *= db2float(outgain_value);
            buffer_output[buffer_stelle] = (buffer.getSample(0, numSamples) + buffer.getSample(1, numSamples)) / 2;
        }
        //*channelData = db2float(ingain_value) * db2float(outgain_value) * *channelData;
        // ..do something to the data...
    }*/
}

//==============================================================================
bool KennlinieAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* KennlinieAudioProcessor::createEditor()
{
    return new KennlinieAudioProcessorEditor (*this);
}

juce::Point<int> KennlinieAudioProcessor::getSavedSize() const
{
    return editorSize;
}

void KennlinieAudioProcessor::setSavedSize(const juce::Point<int>& size)
{
    editorSize = size;
}
float KennlinieAudioProcessor::processingKurve(float input)
{
    float x_value, y_value;
    if (input < 0)
    {
        x_value = xneg_value;
        y_value = yneg_value;
    }
    else
    {
        x_value = xpos_value;
        y_value = ypos_value;
    }
    float ypos = ceil(y_value);
    float float_pi= juce::MathConstants<float>::pi;
    float output=0;
    {
        switch (art) {
        case 1:
        {
            if (y_value==0)
            {
                output = input;
            }
            else
            {
                output = 2 * atan(abs(y_value) * (input + x_value)) / float_pi;
            }
            break;
        }
        case 2:
        {
            if (y_value == 0) {
                output = input;
            }
            else{
                output= atan(10 * y_value * (input + x_value))/ atan(10 * y_value);
            }
            /*float nenner = atan(10 * ypos_value);
            float zaehler = atan(10 * ypos_value * (input + xpos_value));
            if (nenner == 0)
            {
                output = 0;
            }
            else
            {
                output = zaehler / nenner;
            }*/
            //output = zaehler / nenner;
            break;
        }
        case 3:
        {
            if (y_value == 0)
            {
                output = abs(input);
            }
            else
            {
                output = 20 * abs(y_value) / (2 * float_pi) * exp(-(abs(input) - abs(x_value)) / (2));
            }
            if (input < 0)
            {
                output = -output;
            }
            break;
        }
        case 4:
        {
            x_value = abs(x_value);
            y_value = abs(y_value);
            double x[3] = { 0, x_value, 1 };
            double y[3] = { 0,y_value,1 };
            if (y_value == 1 && x_value == 0)
            {
                output = 1;
            }
            else if (y_value == 1 && x_value == 1)
            {
                output = abs(input);
            }
            else if (y_value == 0 && x_value == 1)
            {
                output = -1;
            }
            else if (y_value == 0 && x_value == 0)
            {
                output = abs(input);
            }
            else if (y_value > 0 && x_value == 0)
            {
                double xn[2] = { 0,1 };
                double yn[2] = { y_value,1 };
                output = lagrange(xn, yn, 2, abs(input));
                /*if (input >= 0)
                {
                    output = lagrange(xn,yn, 2, input);
                }
                else
                {
                    output = -lagrange(xn, yn, 2, abs(input));
                }*/
            }
            else if (y_value == 0 && x_value > 0)
            {
                double xn[2] = { x_value,1 };
                double yn[2] = { 0,1 };
                output = lagrange(xn, yn, 2, abs(input));
                /*if (input >= 0)
                {
                    output = lagrange(xn, yn, 2, input);
                }
                else
                {
                    output = -lagrange(xn, yn, 2, abs(input));
                }*/
            }
            else
            {
                output = lagrange(x, y, 3, abs(input));
                /*if (input >= 0)
                {
                    output = lagrange(x, y, 3, input);
                }
                else
                {
                    output = -lagrange(x, y, 3, abs(input));
                }*/
            }
            if (input < 0)
            {
                output = -output;
            }
            break;
        }
        case 5:
        {
            if (y_value == 0)
            {
                output = input;
            }
            else
            {
                output = y_value * exp(-pow(input - x_value, 2) * 10);
            }
            break;
        }
        default:
            output= input;
            break;
        }
        if (input == 0)
        {
            output = 0;
        }
        if (output >= 1)
        {
            output = 1;
        }
        else if (output <= -1)
        {
            output = -1;
        }
        return output;
    }
}


//Source --> will pirkle fxobjects.h
float KennlinieAudioProcessor::lagrange(double* x, double* y, int n, float xbar)
{
    int i, j;
    double fx = 0.0;
    double l = 1.0;
    for (i = 0; i < n; i++)
    {
        l = 1.0;
        for (j = 0; j < n; j++)
        {
            if (j != i)
                l *= (xbar - x[j]) / (x[i] - x[j]);
        }
        fx += l * y[i];
    }
    return (fx);
}

void KennlinieAudioProcessor::setFilter(BiQuad input)
{
    /*input.setSampleRate(getSampleRate());
    input.setFreq(freqc);
    input.setQ(q_value);
    input.setType(filterType);*/
}

//==============================================================================
void KennlinieAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void KennlinieAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KennlinieAudioProcessor();
}
