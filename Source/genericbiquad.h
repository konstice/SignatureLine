#ifndef GENERICBIQUAD_H
#define GENERICBIQUAD_H
#include <vector>



class GenericBiquad
{
public:
    enum FilterType{
        FT_LowPass = 1,
        FT_HighPass,
        FT_BandPass,
        FT_BandStop,
        FT_ByPass
    };
private:
    std::vector<double> m_inputSamples;
    std::vector<double> m_outputSamples;
    std::vector<double> m_parameters;
    double m_cornerFrequency;
    double m_quality;
    double m_sampleRate;
    FilterType m_type;
public:
    GenericBiquad();
    GenericBiquad(const double &sampleRate);

    double processAudio(const double &input);
    void resetBiquad();

    void setCornerFrequency(const double &newCornerFrequency);
    void setQuality(const double &newQuality);
    void setType(const FilterType& newType);
    void setSampleRate(const double& sampleRate);

    double cornerFrequency() const;

    double quality() const;

private:
    void updateParameters();
};

#endif // GENERICBIQUAD_H
