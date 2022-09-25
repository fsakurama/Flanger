/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyFlangerAudioProcessorEditor::MyFlangerAudioProcessorEditor (MyFlangerAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p)
{
    DefaultButton.addListener(this);
    OnButton.addListener(this);
    OffButton.addListener(this);
    OnOffSlider.addListener(this);
    TypeSlider.addListener(this);
    filterMenuSlider.addListener(this);
    
    DryWetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    DryWetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 50);
    addAndMakeVisible(DryWetSlider);
    DryWetSlider.setBounds(100, 0, 150, 150);
    DryWetSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mValueTreeState, "drywet", DryWetSlider);
    DryWetLabel.setText("DryWet", juce::dontSendNotification);
    DryWetLabel.attachToComponent(&DryWetSlider, true);
    addAndMakeVisible(DryWetLabel);
    
    FeedbackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    FeedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 50);
    addAndMakeVisible(FeedbackSlider);
    FeedbackSlider.setBounds(450, 0, 150, 150);
    FeedbackSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mValueTreeState, "feedback", FeedbackSlider);
    FeedbackLabel.setText("Feedback", juce::dontSendNotification);
    FeedbackLabel.attachToComponent(&FeedbackSlider, true);
    addAndMakeVisible(FeedbackLabel);
    
    DepthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    DepthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 50);
    addAndMakeVisible(DepthSlider);
    DepthSlider.setBounds(100, 200, 150, 150);
    DepthSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mValueTreeState, "depth", DepthSlider);
    DepthLabel.setText("Depth", juce::dontSendNotification);
    DepthLabel.attachToComponent(&DepthSlider, true);
    addAndMakeVisible(DepthLabel);
    
    RateSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    RateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 50);
    addAndMakeVisible(RateSlider);
    RateSlider.setBounds(450, 200, 150, 150);
    RateSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mValueTreeState, "rate", RateSlider);
    RateLabel.setText("Rate", juce::dontSendNotification);
    RateLabel.attachToComponent(&RateSlider, true);
    addAndMakeVisible(RateLabel);
    
    TypeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    TypeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 50);
    addAndMakeVisible(TypeSlider);
    TypeSlider.setBounds(450, 400, 150, 150);
    TypeSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mValueTreeState, "type", TypeSlider);
    TypeLabel.setText("Type", juce::dontSendNotification);
    TypeLabel.attachToComponent(&TypeSlider, true);
    addAndMakeVisible(TypeLabel);
    
    TypeFlanger.setText("Flanger", juce::dontSendNotification);
    TypeFlanger.setBounds(490, 470, 100, 100);
    addAndMakeVisible(TypeFlanger);
    TypeFlanger.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    TypeFlanger.setColour(juce::Label::textColourId, juce::Colours::white);
    TypeChorus.setText("Chorus", juce::dontSendNotification);
    TypeChorus.setBounds(490, 470, 100, 100);
    //addAndMakeVisible(TypeChorus);
    TypeChorus.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    TypeChorus.setColour(juce::Label::textColourId, juce::Colours::white);
    
    otherLookAndFeel.setColour(juce::Slider::thumbColourId, juce::Colours::red);
    DryWetSlider.setLookAndFeel(&otherLookAndFeel);
    FeedbackSlider.setLookAndFeel(&otherLookAndFeel);
    DepthSlider.setLookAndFeel(&otherLookAndFeel);
    RateSlider.setLookAndFeel(&otherLookAndFeel);
    TypeSlider.setLookAndFeel(&otherLookAndFeel);
    
    DefaultButton.setBounds(150, 420, 100, 100);
    DefaultButton.setButtonText("Default Value");
    addAndMakeVisible(DefaultButton);
    
    OnOffSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    OnOffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 50);
    //addAndMakeVisible(OnOffSlider);
    OnOffSlider.setBounds(500, 550, 150, 150);
    OnOffSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mValueTreeState, "onoff", OnOffSlider);
    addAndMakeVisible(OnButton);
    OnButton.setBounds(200, 570, 100, 100);
    OnButton.setButtonText("ON");
    addAndMakeVisible(OffButton);
    OffButton.setBounds(400, 570, 100, 100);
    OffButton.setButtonText("OFF");

    filterCutoffSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterCutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 50);
    addAndMakeVisible(filterCutoffSlider);
    filterCutoffSlider.setBounds(800, 0, 150, 150);
    filterCutoffSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mValueTreeState, "cutoff", filterCutoffSlider);
    filterCutoffLabel.setText("CutOff", juce::dontSendNotification);
    filterCutoffLabel.attachToComponent(&filterCutoffSlider, true);
    addAndMakeVisible(filterCutoffLabel);

    filterResSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterResSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 50);
    addAndMakeVisible(filterResSlider);
    filterResSlider.setBounds(800, 200, 150, 150);
    filterResSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mValueTreeState, "resonance", filterResSlider);
    filterResLabel.setText("Resonance", juce::dontSendNotification);
    filterResLabel.attachToComponent(&filterResSlider, true);
    addAndMakeVisible(filterResLabel);
    
    filterMenuSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    filterMenuSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 50);
    addAndMakeVisible(filterMenuSlider);
    filterMenuSlider.setBounds(800, 400, 150, 150);
    filterMenuSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mValueTreeState, "filtermenu", filterMenuSlider);
    filterMenuLabel.setText("Filter", juce::dontSendNotification);
    filterMenuLabel.attachToComponent(&filterMenuSlider, true);
    addAndMakeVisible(filterMenuLabel);
    
    filterLowpass.setText("LowPass", juce::dontSendNotification);
    filterLowpass.setBounds(840, 470, 100, 100);
    addAndMakeVisible(filterLowpass);
    filterLowpass.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    filterLowpass.setColour(juce::Label::textColourId, juce::Colours::white);
    filterBandpass.setText("BandPass", juce::dontSendNotification);
    filterBandpass.setBounds(840, 470, 100, 100);
    //addAndMakeVisible(filterBandpass);
    filterBandpass.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    filterBandpass.setColour(juce::Label::textColourId, juce::Colours::white);
    filterHighpass.setText("HighPass", juce::dontSendNotification);
    filterHighpass.setBounds(840, 470, 100, 100);
    //addAndMakeVisible(filterHighpass);
    filterHighpass.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    filterHighpass.setColour(juce::Label::textColourId, juce::Colours::white);
    
    setSize (1000, 800);
}

MyFlangerAudioProcessorEditor::~MyFlangerAudioProcessorEditor()
{
}

//==============================================================================
void MyFlangerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void MyFlangerAudioProcessorEditor::resized()
{
   
}

void MyFlangerAudioProcessorEditor::buttonClicked(juce::Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == &DefaultButton)
    {
        DryWetSlider.setValue(0.5);
        FeedbackSlider.setValue(0.5);
        DepthSlider.setValue(0.5);
        RateSlider.setValue(10.0);
        //PhaseOffsetSlider.setValue(0.0);
    }
    
    if (buttonThatWasClicked == &OnButton)
    {
        OnOffSlider.setValue(true);
    }
    if (buttonThatWasClicked == &OffButton)
    {
        OnOffSlider.setValue(false);
    }
}

void MyFlangerAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &TypeSlider)
    {
        auto value = slider->getValue();
        if(value == 0)
        {
            addAndMakeVisible(FeedbackSlider);
            addAndMakeVisible(FeedbackLabel);
            addAndMakeVisible(TypeFlanger);
            TypeChorus.setVisible(false);
        }
        if(value == 1)
        {
            FeedbackSlider.setVisible(false);
            FeedbackLabel.setVisible(false);
            addAndMakeVisible(TypeChorus);
            TypeFlanger.setVisible(false);
        }
    }
    
    if (slider == &OnOffSlider)
    {
        auto state = slider->getValue();
        if(state == true)
        {
            OnButton.setColour(0x1000100, juce::Colours::red);
            OffButton.setColour(0x1000100, juce::Colours::grey);
        }
        if(state == false)
        {
            OnButton.setColour(0x1000100, juce::Colours::grey);
            OffButton.setColour(0x1000100, juce::Colours::red);
        }
    }
    
    if (slider == &filterMenuSlider)
    {
        auto menue = slider->getValue();
        if(menue == 0)
        {
            addAndMakeVisible(filterLowpass);
            Nofilter.setVisible(false);
            filterBandpass.setVisible(false);
            filterHighpass.setVisible(false);
        }
        if(menue == 1)
        {
            addAndMakeVisible(filterBandpass);
            Nofilter.setVisible(false);
            filterLowpass.setVisible(false);
            filterHighpass.setVisible(false);
        }
        if(menue == 2)
        {
            addAndMakeVisible(filterHighpass);
            Nofilter.setVisible(false);
            filterLowpass.setVisible(false);
            filterBandpass.setVisible(false);
        }
    }
}
