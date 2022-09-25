/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyFlangerAudioProcessor::MyFlangerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), mValueTreeState(*this, nullptr, "Parameters", CreateParameters())
#endif
{
    mDelayTimeSmooth = 0;
    mDryWet = 0.5;
    mCircularBufferL = nullptr;
    mCircularBufferR = nullptr;
    mCircularBufferWriteHead = 0;
    mCircularBufferLength = 0;
    mDelayTimeInSamples = 0;
    mDelayReadHead = 0;
        
    mFeedbackLeft = 0;
    mFeedbackRight = 0;
}

MyFlangerAudioProcessor::~MyFlangerAudioProcessor()
{
    if (mCircularBufferL != nullptr){
        delete [] mCircularBufferL;
        mCircularBufferL = nullptr;
    }
    if (mCircularBufferL != nullptr){
        delete [] mCircularBufferL;
        mCircularBufferL = nullptr;
    }
}

//==============================================================================
const juce::String MyFlangerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MyFlangerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MyFlangerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MyFlangerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MyFlangerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MyFlangerAudioProcessor::getNumPrograms()
{
    return 1;
}

int MyFlangerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MyFlangerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MyFlangerAudioProcessor::getProgramName (int index)
{
    return {};
}

void MyFlangerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MyFlangerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    lastSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = lastSampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getMainBusNumOutputChannels();

    stateVariableFilter.reset();
    updateFilter();
    stateVariableFilter.prepare(spec);
    
    mCircularBufferLength = sampleRate * MAX_DELAY_TIME;
    
    auto RateParameterValue = mValueTreeState.getRawParameterValue("rate");
    
    mDelayTimeInSamples = sampleRate * *RateParameterValue;
    
    //mDelayTimeSmooth = *RateParameterValue;
    mDelayTimeSmooth = 1;
    if (mCircularBufferL == nullptr) {
        mCircularBufferL = new float[mCircularBufferLength];
    }
        
    juce::zeromem(mCircularBufferL, mCircularBufferLength * sizeof(float));
        
    if (mCircularBufferR == nullptr) {
        mCircularBufferR = new float[mCircularBufferLength];
    }
        
    juce::zeromem(mCircularBufferL, mCircularBufferLength * sizeof(float));
    
    mCircularBufferWriteHead = 0;
    
    mDelayReadHead = 0;
}

void MyFlangerAudioProcessor::releaseResources()
{
    
}

void MyFlangerAudioProcessor::updateFilter()
{
    auto menuChoice = mValueTreeState.getRawParameterValue("filtermenu");
    auto cutoff = mValueTreeState.getRawParameterValue("cutoff");
    auto res = mValueTreeState.getRawParameterValue("resonance");
    
    if (*menuChoice == 0)//low pass
    {
        stateVariableFilter.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::lowPass;

        stateVariableFilter.state->setCutOffFrequency (lastSampleRate, *cutoff, *res);
    }

    if (*menuChoice == 1)// band pass
    {
        stateVariableFilter.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::bandPass;

        stateVariableFilter.state->setCutOffFrequency (lastSampleRate, *cutoff, *res);
    }

    if (*menuChoice == 2)// high pass
    {
        stateVariableFilter.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::highPass;

        stateVariableFilter.state->setCutOffFrequency (lastSampleRate, *cutoff, *res);
    }

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MyFlangerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MyFlangerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    const int numSamples = buffer.getNumSamples();
    
    auto OnOffParameterValue = mValueTreeState.getRawParameterValue("onoff");
    
    if (*OnOffParameterValue == false){//off
        juce::ScopedNoDenormals noDenormals;
            auto totalNumInputChannels  = getTotalNumInputChannels();
            auto totalNumOutputChannels = getTotalNumOutputChannels();

            for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
                buffer.clear (i, 0, buffer.getNumSamples());
    }
    
    if (*OnOffParameterValue == true){//on
    
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    juce::dsp::AudioBlock<float> block (buffer);

    updateFilter();
    stateVariableFilter.process(juce::dsp::ProcessContextReplacing<float> (block));
    
    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);
    
    for (int i = 0; i < buffer.getNumSamples(); i++) {
        auto RateParameterValue = mValueTreeState.getRawParameterValue("rate");
        auto DepthParameterValue = mValueTreeState.getRawParameterValue("depth");
        
        float lfoOut = sin(2 * M_PI * mLFOPhase);
        mLFOPhase += *RateParameterValue / getSampleRate();
        if (mLFOPhase > 1){
            mLFOPhase -= 1;
        }
        
        lfoOut *= *DepthParameterValue;
        
        auto TypeParameterValue = mValueTreeState.getRawParameterValue("type");
        if(*TypeParameterValue == 0){
            float lfoOutMapped =juce::jmap(lfoOut, -1.0f, 1.0f, 0.001f, 0.005f);//flanger
            mDelayTimeSmooth = mDelayTimeSmooth - 0.001 * (mDelayTimeSmooth - lfoOutMapped);
            mDelayTimeInSamples = getSampleRate() * mDelayTimeSmooth;
                
            mCircularBufferL[mCircularBufferWriteHead] = leftChannel[i] + mFeedbackLeft;
            mCircularBufferR[mCircularBufferWriteHead] = rightChannel[i] + mFeedbackRight;
            
            mDelayReadHead = mCircularBufferWriteHead - mDelayTimeInSamples;
                
            if (mDelayReadHead < 0){
                mDelayReadHead += mCircularBufferLength;
            }
                
            int readHead_x = (int)mDelayReadHead;
            float readHeadFloat = mDelayReadHead - readHead_x;
                
            int readHead_x1 = readHead_x + 1;
            if (readHead_x1 >= mCircularBufferLength){
                readHead_x1 -= mCircularBufferLength;
            }
                
                
            float delay_sample_left = lin_interp(mCircularBufferL[readHead_x], mCircularBufferL[readHead_x1], readHeadFloat);
            float delay_sample_right = lin_interp(mCircularBufferR[readHead_x], mCircularBufferR[readHead_x1], readHeadFloat);
            
            auto FeedbackParameterValue = mValueTreeState.getRawParameterValue("feedback");
                
            mFeedbackLeft = delay_sample_left * *FeedbackParameterValue;
            mFeedbackRight = delay_sample_right * *FeedbackParameterValue;
            
            auto DryWetParameterValue = mValueTreeState.getRawParameterValue("drywet");
                
            buffer.setSample(0, i, buffer.getSample(0, i) * *DryWetParameterValue + delay_sample_left * (1 - *DryWetParameterValue));
            buffer.setSample(1, i, buffer.getSample(1, i) * *DryWetParameterValue + delay_sample_right * (1 - *DryWetParameterValue));
            
            mCircularBufferWriteHead++; //update the write position
                
            if(mCircularBufferWriteHead >= mCircularBufferLength){
               mCircularBufferWriteHead = 0; // fold back to 0 if our position is bigger than buffer length
            }
        }
        if(*TypeParameterValue == 1){
            float lfoOutMapped =juce::jmap(lfoOut, -1.0f, 1.0f, 0.01f, 0.035f);//chorus
            mDelayTimeSmooth = mDelayTimeSmooth - 0.001 * (mDelayTimeSmooth - lfoOutMapped);
            mDelayTimeInSamples = getSampleRate() * mDelayTimeSmooth;
                
            mCircularBufferL[mCircularBufferWriteHead] = leftChannel[i];
            mCircularBufferR[mCircularBufferWriteHead] = rightChannel[i];
            
            mDelayReadHead = mCircularBufferWriteHead - mDelayTimeInSamples;
                
            if (mDelayReadHead < 0){
                mDelayReadHead += mCircularBufferLength;
            }
                
            int readHead_x = (int)mDelayReadHead;
            float readHeadFloat = mDelayReadHead - readHead_x;
                
            int readHead_x1 = readHead_x + 1;
            if (readHead_x1 >= mCircularBufferLength){
                readHead_x1 -= mCircularBufferLength;
            }
                
                
            float delay_sample_left = lin_interp(mCircularBufferL[readHead_x], mCircularBufferL[readHead_x1], readHeadFloat);
            float delay_sample_right = lin_interp(mCircularBufferR[readHead_x], mCircularBufferR[readHead_x1], readHeadFloat);
            
            //auto FeedbackParameterValue = mValueTreeState.getRawParameterValue("feedback");
                
            //mFeedbackLeft = delay_sample_left * *FeedbackParameterValue;
            //mFeedbackRight = delay_sample_right * *FeedbackParameterValue;
            
            auto DryWetParameterValue = mValueTreeState.getRawParameterValue("drywet");
                
            buffer.setSample(0, i, buffer.getSample(0, i) * *DryWetParameterValue + delay_sample_left * (1 - *DryWetParameterValue));
            buffer.setSample(1, i, buffer.getSample(1, i) * *DryWetParameterValue + delay_sample_right * (1 - *DryWetParameterValue));
            
            mCircularBufferWriteHead++; //update the write position
                
            if(mCircularBufferWriteHead >= mCircularBufferLength){
               mCircularBufferWriteHead = 0; // fold back to 0 if our position is bigger than buffer length
            }
        }
    }
    }//OnOff end
}//processBlock end

//==============================================================================
bool MyFlangerAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* MyFlangerAudioProcessor::createEditor()
{
    return new MyFlangerAudioProcessorEditor (*this);
}

//==============================================================================
void MyFlangerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = mValueTreeState.copyState();
         std::unique_ptr<juce::XmlElement> myXML (state.createXml());
         copyXmlToBinary(*myXML, destData);
}

void MyFlangerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary(data, sizeInBytes));
        
    if(xmlState.get() != nullptr)
        if(xmlState->hasTagName(mValueTreeState.state.getType()))
            mValueTreeState.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyFlangerAudioProcessor();
}

float MyFlangerAudioProcessor::lin_interp(float sample_x, float sample_x1, float inPhase)
{
    return (1 - inPhase) * sample_x + inPhase * sample_x1;
}

juce::AudioProcessorValueTreeState::ParameterLayout MyFlangerAudioProcessor::CreateParameters(){
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drywet", //Parameter ID
                                                                 "DryWet", //Parameter Name
                                                                 0.0f,     //Min Value
                                                                 1.0f,     //Max Value
                                                                 0.5f      //Default Value
                                                                 ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("feedback",
                                                                 "Feedback",
                                                                 0.0f,
                                                                 0.98f,
                                                                 0.5f
                                                                 ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("depth",
                                                                 "Depth",
                                                                 0.0,
                                                                 1.0,
                                                                 0.5));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("rate",
                                                                "Rate",
                                                                0.1f,
                                                                20.f,
                                                                10.f));
        
    params.push_back(std::make_unique<juce::AudioParameterInt>("type",
                                                              "Type",
                                                              0,
                                                              1,
                                                              0));
    params.push_back(std::make_unique<juce::AudioParameterBool>("onoff", "OnOff", true));
    
    params.push_back(std::make_unique<juce::AudioParameterInt>("filtermenu", "Filter Menu", 0, 2, 0));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("cutoff", "CutOff", 20.0f, 20000.0f, 600.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("resonance", "Resonance", 1.0f, 5.0f, 1.0f));
    
    return {params.begin(), params.end()};
}

