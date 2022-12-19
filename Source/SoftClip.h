#include <JuceHeader.h>
#include <iostream>
#include <vector>
#include <math.h>

#pragma once

class SoftClipper
{
public:
    SoftClipper();

    void prepare(juce::dsp::ProcessSpec& spec);
    
    void processBlock(juce::dsp::AudioBlock<float>& block)
    {
        for (int ch = 0; ch < block.getNumChannels(); ++ch)
        {
            float* data = block.getChannelPointer(ch);

            for (int sample = 0; sample < block.getNumSamples(); ++sample)
            {
                // This Soft Clipper is based off of Eric Tarr's example in the book 'Hack Audio'
                data[sample] = 2.0 / M_PI * std::atan(data[sample] * m_Drive.getNextValue());
            }
        }
    }

    void setDrive(float newDrive);

private:
    
   float m_SampleRate;
   juce::SmoothedValue<float> m_Drive;

};
