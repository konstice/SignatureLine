#include "genericbiquad.h"
#include <math.h>
#define M_PI                  3.14159265358979323846

double GenericBiquad::cornerFrequency() const
{
    return m_cornerFrequency;
}

double GenericBiquad::quality() const
{
    return m_quality;
}

GenericBiquad::GenericBiquad():
    GenericBiquad(44100.0)
{
    
}

GenericBiquad::GenericBiquad(const double &sampleRate):
    m_inputSamples({0,0}),
    m_outputSamples({0,0}),
    m_parameters({1,0,0,0,0}),
    m_cornerFrequency(100),
    m_quality(0.707),
    m_sampleRate(sampleRate),
    m_type(FT_ByPass)
{

}

double GenericBiquad::processAudio(const double &input)
{
    double output = m_parameters[0] * input + m_parameters[1] * m_inputSamples[0] + m_parameters[2] * m_inputSamples[1] -
            m_parameters[3] * m_outputSamples[0] - m_parameters[4] * m_outputSamples[1];

    m_inputSamples[1] = m_inputSamples[0];
    m_inputSamples[0] = input;

    m_outputSamples[1] = m_outputSamples[0];
    m_outputSamples[0] = output;

    return output;
}

void GenericBiquad::resetBiquad()
{
    m_inputSamples={0,0};
    m_outputSamples={0,0};
    m_parameters={1,0,0,0,0};
    m_cornerFrequency = 100;
    m_quality = 0.707;
    m_sampleRate = 44100;
    m_type = FT_ByPass;
}

void GenericBiquad::setCornerFrequency(const double &newCornerFrequency)
{
    m_cornerFrequency = newCornerFrequency;
    updateParameters();
}

void GenericBiquad::setQuality(const double &newQuality)
{
    m_quality = newQuality;
    updateParameters();
}

void GenericBiquad::setType(const FilterType &newType)
{
    m_type = newType;
    updateParameters();
}

void GenericBiquad::setSampleRate(const double& sampleRate)
{
    m_sampleRate = sampleRate;
    updateParameters();
}

void GenericBiquad::updateParameters()
{
    double rhoc = 2 * M_PI * m_cornerFrequency / m_sampleRate;
    switch (m_type)
    {
    case FT_LowPass:
    {
        double d = 1 / m_quality;
        double beta = (1 - d * sin(rhoc) / 2) / (2 * (1 + d * sin(rhoc) / 2));
        double gamma = (0.5 + beta) * cos(rhoc);
        double zaehler = 0.5 + beta - gamma;
        m_parameters[0] = zaehler / 2;
        m_parameters[1] = zaehler;
        m_parameters[2] = m_parameters[0];
        m_parameters[3] = -2 * gamma;
        m_parameters[4] = 2 * beta;
        break;
    }
    case FT_HighPass:
    {
        double d = 1 / m_quality;
        double beta = (1 - d * sin(rhoc) / 2) / (2 * (1 + d * sin(rhoc) / 2));
        double gamma = (0.5 + beta) * cos(rhoc);
        double zaehler = 0.5 + beta + gamma;
        m_parameters[0] = zaehler / 2;
        m_parameters[1] = -zaehler;
        m_parameters[2] = m_parameters[0];
        m_parameters[3] = -2 * gamma;
        m_parameters[4] = 2 * beta;
        break;
    }
    case FT_BandPass:
    {
        double tan_arg = rhoc / (2 * m_quality);
        if (tan_arg >= 0.95 * M_PI / 2)
        {
            tan_arg = 0.95 * M_PI / 2;
        }
        double beta = (1 - tan(tan_arg)) / (2 * (1 + tan(tan_arg)));
        double gamma = (0.5 + beta) * cos(rhoc);
        m_parameters[0] = 0.5 - beta;
        m_parameters[1] = 0;
        m_parameters[2] = -m_parameters[0];
        m_parameters[3] = -2 * gamma;
        m_parameters[4] = 2 * beta;
        break;
    }
    case FT_BandStop:
    {
        double tan_arg = rhoc / (2 * m_quality);
        if (tan_arg >= 0.95 * M_PI / 2)
        {
            tan_arg = 0.95 * M_PI / 2;
        }
        double beta = (1 - tan(tan_arg)) / (2 * (1 +tan(tan_arg)));
        double gamma = (0.5 + beta) * cos(rhoc);
        m_parameters[0] = 0.5 + beta;
        m_parameters[1] = -2*beta;
        m_parameters[2] = m_parameters[0];
        m_parameters[3] = -2 * gamma;
        m_parameters[4] = 2 * beta;
        break;
    }
    default:
    {
        m_parameters={1,0,0,0,0};
        break;
    }
    }
}
