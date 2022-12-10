/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RaumDelayDistortionAudioProcessorEditor::RaumDelayDistortionAudioProcessorEditor (RaumDelayDistortionAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
    titleDelay.setText("Delay", juce::dontSendNotification);
    
    addAndMakeVisible(titleDelay);
    addAndMakeVisible(leftDelayTimeSlider);
    addAndMakeVisible(righDelayTimeSlider);
    addAndMakeVisible(lfoSpeedSlider);
    addAndMakeVisible(lfoAmountSlider);
    addAndMakeVisible(pingPongToggle);

    titleDistortion.setText("Distortion", juce::dontSendNotification);

    addAndMakeVisible(titleDistortion);
    addAndMakeVisible(gainAmountSlider);
    
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

RaumDelayDistortionAudioProcessorEditor::~RaumDelayDistortionAudioProcessorEditor()
{
}

//==============================================================================
void RaumDelayDistortionAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    
}

void RaumDelayDistortionAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    auto bounds = getLocalBounds();
    
    auto unitHeight = bounds.getHeight() * 0.125;
    auto margin = 4;
    
    titleDelay.setBounds(bounds.removeFromTop(unitHeight).reduced(margin));
    
    leftDelayTimeSlider.setBounds(bounds.removeFromTop(unitHeight).reduced(margin));
    righDelayTimeSlider.setBounds(bounds.removeFromTop(unitHeight).reduced(margin));
    lfoSpeedSlider.setBounds(bounds.removeFromTop(unitHeight).reduced(margin));
    lfoAmountSlider.setBounds(bounds.removeFromTop(unitHeight).reduced(margin));
    
    pingPongToggle.setBounds(bounds.removeFromTop(unitHeight).reduced(margin));


    titleDistortion.setBounds(bounds.removeFromTop(unitHeight).reduced(margin));

    gainAmountSlider.setBounds(bounds.removeFromTop(unitHeight).reduced(margin));

    
}
