/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#define MAX_DELAY_TIME 2

//==============================================================================
/**
*/
class MyFlangerAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    MyFlangerAudioProcessor();
    ~MyFlangerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void updateFilter();

    void updateParameters ();
    
    float lin_interp(float sample_x, float sample_x1, float inPhase);
    
    juce::AudioProcessorValueTreeState mValueTreeState;
    
private:
    
    juce::AudioProcessorValueTreeState::ParameterLayout CreateParameters();
    
    juce::dsp::ProcessorDuplicator<juce::dsp::StateVariableFilter::Filter<float>, juce::dsp::StateVariableFilter::Parameters<float>> stateVariableFilter;

    float lastSampleRate;
    
    float mLFOPhase;
    
    float mDelayTimeSmooth;
    
    juce::AudioParameterFloat* mDryWetParameter;
    juce::AudioParameterFloat* mFeedbackParameter;//REGENERATION/RESONANCE
    juce::AudioParameterFloat* mDepthParameter;//WIDTH
    juce::AudioParameterFloat* mRateParameter;//SPEED
    
    juce::AudioParameterInt* mTypeParameter;
    juce::AudioParameterBool* mOnOffParameter;

    
    float mDryWet;
        
    float mFeedbackLeft;
    float mFeedbackRight;
    
    float* mCircularBufferL;
    float* mCircularBufferR;

    int mCircularBufferWriteHead;
    int mCircularBufferLength;

    float mDelayTimeInSamples;
    float mDelayReadHead;
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyFlangerAudioProcessor)
};
