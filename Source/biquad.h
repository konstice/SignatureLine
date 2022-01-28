/*
  ==============================================================================

    biquad.h
    Created: 9 Jul 2020 4:57:42pm
    Author:  kks

    Contains the biquad function for eq

    source: willpirkle.com

  ==============================================================================
*/

#pragma once
#define FLT_MIN_PLUS          1.175494351e-38         /* min positive value */
#define FLT_MIN_MINUS        -1.175494351e-38         /* min negative value */

class BiQuad
{
public:
    //BiQuad(void);
    virtual ~BiQuad(void) {};
protected:
    float in_z1;
    float in_z2;
    float out_z1;
    float out_z2;
    float a0, a1, a2, b1, b2;
    float freqc, q, fs;
    int type;
public:
    float float_pi = juce::MathConstants<float>::pi;
    float tau = 2 * float_pi;

    BiQuad(void):
        in_z1 = 0,
        in_z2 = 0,
        out_z1 = 0,
        out_z2 = 0,
        q = 0.707,
        freqc = 100,
        type = 0,
        fs = 44100,
        a0 = 1,
        a1 = 0,
        a2 = 0,
        b1 = 0,
        b2 = 0
    {
        
    }

    void init(float sampleRate, int filterType, float eckfreq, float guete)
    {
        fs = sampleRate;
        type = filterType;
        freqc = eckfreq;
        q = guete;
    }

    void reset()
    {
        in_z1 = 0;
        in_z2 = 0;
        out_z1 = 0;
        out_z2 = 0;
    }

    int getType()
    {
        return type;
    }

    float getFreq() {
        return freqc;
    }

    float process(float input)
    {
        getParameters();
        float output = a0 * input + a1 * in_z1 + a2 * in_z2 - b1 * out_z1 - b2 * out_z2;

        if (output > 0.0 && output < FLT_MIN_PLUS) output = 0;
        if (output < 0.0 && output > FLT_MIN_MINUS) output = 0;

        out_z2 = out_z1;
        out_z1 = output;

        in_z2 = in_z1;
        in_z1 = input;

        return output;
    }

    void getParameters()
    {
        switch (type)
        {
            //lpf
        case 1:
        {
            float rhoc = tau * freqc / fs;
            float d = 1 / q;
            float beta = (1 - d * sin(rhoc) / 2) / (2 * (1 + d * sin(rhoc) / 2));
            float gamma = (0.5 + beta) * cos(rhoc);
            float zaehler = 0.5 + beta - gamma;
            a0 = zaehler / 2;
            a1 = zaehler;
            a2 = a0;
            b1 = -2 * gamma;
            b2 = 2 * beta;
            break;
        }
        //hpf
        case 2:
        {
            float rhoc = tau * freqc / fs;
            float d = 1 / q;
            float beta = (1 - d * sin(rhoc) / 2) / (2 * (1 + d * sin(rhoc) / 2));
            float gamma = (0.5 + beta) * cos(rhoc);
            float zaehler = 0.5 + beta + gamma;
            a0 = zaehler / 2;
            a1 = -zaehler;
            a2 = a0;
            b1 = -2 * gamma;
            b2 = 2 * beta;
            break;
        }
        //bpf
        case 3:
        {
            float rhoc = tau * freqc / fs;
            float d = 1 / q;
            float tan_arg = rhoc / (2 * q);
            if (tan_arg >= 0.95 * float_pi / 2)
            {
                tan_arg = 0.95 * float_pi / 2;
            }
            float beta = (1 - tan(tan_arg)) / (2 * (1 + tan(tan_arg)));
            float gamma = (0.5 + beta) * cos(rhoc);
            a0 = 0.5 - beta;
            a1 = 0;
            a2 = -a0;
            b1 = -2 * gamma;
            b2 = 2 * beta;
            break;
        }
        case 4:
        {
            float rhoc = tau * freqc / fs;
            float d = 1 / q;
            float tan_arg = rhoc / (2 * q);
            if (tan_arg >= 0.95 * float_pi / 2)
            {
                tan_arg = 0.95 * float_pi / 2;
            }
            float beta = (1 - tan(tan_arg)) / (2 * (1 +tan(tan_arg)));
            float gamma = (0.5 + beta) * cos(rhoc);
            a0 = 0.5 + beta;
            a1 = -2*beta;
            a2 = a0;
            b1 = -2 * gamma;
            b2 = 2 * beta;
            break;
        }
        default:
        {
            a0 = 1;
            a1 = 0;
            a2 = 0;
            b1 = 0;
            b2 = 0;
            break;
        }
        }
    }

};

/*class Puffer
{
public:
    Puffer() {};
    ~Puffer() {};

    reset

};*/