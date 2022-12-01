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
class RaumDelayDistortionAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    RaumDelayDistortionAudioProcessorEditor (RaumDelayDistortionAudioProcessor&);
    ~RaumDelayDistortionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RaumDelayDistortionAudioProcessor& audioProcessor;

    juce::Slider leftDelayTimeSlider,righDelayTimeSlider,lfoSpeedSlider,lfoAmountSlider;
    juce::ToggleButton pingPongToggle;
    juce::Label title;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RaumDelayDistortionAudioProcessorEditor)
};
