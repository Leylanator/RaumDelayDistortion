/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RaumDelayDistortionAudioProcessor::RaumDelayDistortionAudioProcessor()
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
,pluginState(*this,nullptr, "PARAMETERS", {
    std::make_unique<juce::AudioParameterFloat>("Delay Time Left","Delay Time Left ",juce::NormalisableRange<float>(0.001,5),0.1,juce::AudioParameterFloatAttributes().withLabel("MS")),
    std::make_unique<juce::AudioParameterFloat>("Delay Time Right","Delay Time Right ",juce::NormalisableRange<float>(0.001,5),0.1,juce::AudioParameterFloatAttributes().withLabel("MS")),
    std::make_unique<juce::AudioParameterFloat>("Feedback","Feedback ",juce::NormalisableRange<float>(0.,1.),0.33,juce::AudioParameterFloatAttributes().withLabel("%").withStringFromValueFunction([](auto x, auto){return juce::String(x*100);})),
    std::make_unique<juce::AudioParameterBool>("Ping Pong","Ping Pong ",false,juce::AudioParameterBoolAttributes().withStringFromValueFunction ([] (auto x, auto) { return x ? "On" : "Off"; }).withLabel ("enabled")),
    std::make_unique<juce::AudioParameterFloat>("LFO Speed","LFO Speed ",juce::NormalisableRange<float>(0.,2),0.33,juce::AudioParameterFloatAttributes().withLabel("HZ")),
    std::make_unique<juce::AudioParameterFloat>("LFO Amount","LFO Amount ",juce::NormalisableRange<float>(0.,1.),0.,juce::AudioParameterFloatAttributes().withLabel("%").withStringFromValueFunction([](auto x, auto){return juce::String(x*100);})),
        std::make_unique<juce::AudioParameterFloat>("Gain", "Gain ",juce::NormalisableRange<float>(0.0f, 30.0f),10.0f,juce::AudioParameterFloatAttributes().withLabel("dB")),
})
{
    pluginState.addParameterListener("Gain", this);

}


RaumDelayDistortionAudioProcessor::~RaumDelayDistortionAudioProcessor()
{
    pluginState.removeParameterListener("Gain", this);
}

void RaumDelayDistortionAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{
//    gainModule.setGainDecibels(pluginState.getRawParameterValue("Gain")->load());
    softClipperModule.setDrive(pluginState.getRawParameterValue("Gain")->load());

}

//==============================================================================
const juce::String RaumDelayDistortionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RaumDelayDistortionAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool RaumDelayDistortionAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool RaumDelayDistortionAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double RaumDelayDistortionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RaumDelayDistortionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int RaumDelayDistortionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RaumDelayDistortionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String RaumDelayDistortionAudioProcessor::getProgramName (int index)
{
    return {};
}

void RaumDelayDistortionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void RaumDelayDistortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    
//    gainModule.prepare(spec); 
//    gainModule.setRampDurationSeconds(0.02);    // Avoids popping when value is changed, say during moving a slider or knob
//    gainModule.setGainDecibels(pluginState.getRawParameterValue("input")->load());

    softClipperModule.prepare(spec);
    softClipperModule.setDrive(pluginState.getRawParameterValue("Gain")->load());
}

void RaumDelayDistortionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
   
//    gainModule.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RaumDelayDistortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
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

void RaumDelayDistortionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        
        // ..do something to the data...
    }
    
    juce::dsp::AudioBlock<float> audioBlock {buffer};

//    gainModule.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    
    softClipperModule.processBlock(audioBlock);

}

//==============================================================================
bool RaumDelayDistortionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* RaumDelayDistortionAudioProcessor::createEditor()
{
    return new RaumDelayDistortionAudioProcessorEditor (*this);
}

//==============================================================================
void RaumDelayDistortionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void RaumDelayDistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RaumDelayDistortionAudioProcessor();
}
