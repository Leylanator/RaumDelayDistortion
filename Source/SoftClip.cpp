#include "SoftClip.h"

SoftClipper::SoftClipper() : m_SampleRate(48000.0f), m_Drive(1.0)
{
    
}

void SoftClipper::prepare(juce::dsp::ProcessSpec &spec)
{
   m_SampleRate = spec.sampleRate;
   m_Drive.reset(m_SampleRate, 0.02);
}

void SoftClipper::setDrive(float newDrive)
{
    m_Drive.setTargetValue(juce::Decibels::decibelsToGain(newDrive));
}
