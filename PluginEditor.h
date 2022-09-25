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
class MyFlangerAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Button::Listener, juce::Slider::Listener
{
public:
    MyFlangerAudioProcessorEditor (MyFlangerAudioProcessor&);
    ~MyFlangerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;
    void sliderValueChanged (juce::Slider* slider) override;

private:
    
    juce::Slider FeedbackSlider;
    juce::Label FeedbackLabel;
    juce::Slider DryWetSlider;
    juce::Label DryWetLabel;
    juce::Slider DepthSlider;
    juce::Label DepthLabel;
    juce::Slider RateSlider;
    juce::Label RateLabel;
    juce::Slider TypeSlider;
    juce::Label TypeLabel;
    juce::Label TypeFlanger;
    juce::Label TypeChorus;
    juce::TextButton DefaultButton;
    juce::Slider OnOffSlider;
    juce::Label DefaultButtonLabel;
    juce::TextButton OnButton;
    juce::Label OnButtonLabel;
    juce::TextButton OffButton;
    juce::Label OffButtonLabel;
    
    juce::Slider filterCutoffSlider;
    juce::Label filterCutoffLabel;
    juce::Slider filterResSlider;
    juce::Label filterResLabel;
    juce::Slider filterMenuSlider;
    juce::Label filterMenuLabel;
    juce::Label Nofilter;
    juce::Label filterLowpass;
    juce::Label filterBandpass;
    juce::Label filterHighpass;
    
    juce::LookAndFeel_V4 otherLookAndFeel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> DryWetSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> FeedbackSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> DepthSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> RateSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> PhaseOffsetSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> TypeSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> OnOffSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterMenuSliderAttachment;
    
    MyFlangerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyFlangerAudioProcessorEditor)
};
