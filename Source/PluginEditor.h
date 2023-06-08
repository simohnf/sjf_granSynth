/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "../sjf_audio/sjf_LookAndFeel.h"
#include "../sjf_audio/sjf_widgets.h"
//==============================================================================
/**
*/


class Sjf_granSynthAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    Sjf_granSynthAudioProcessorEditor (Sjf_granSynthAudioProcessor&);
    ~Sjf_granSynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void randomiseGraphs();
    void getGraphsAsVectors();
    void triggerNewCloud();
    void randomiseVariables();
private:
    
    juce::TextButton loadSampleButton, triggerRandomCloudButton, triggerCloudButton, randomGraphsButton;
    juce::ComboBox envTypeBox;
    sjf_numBox cloudLengthNumBox;
    sjf_grapher grainPositionGraph, grainPanGraph, grainTransposeGraph, grainSizeGraph, grainGainGraph, grainDeltaGraph, grainReverbGraph;
    juce::Label /*revSizeLabel, revDampingLabel, sampleNameLabel,*/ tooltipLabel;
    juce::Slider reverbSizeSlider, reverbDampingSlider;
    Sjf_granSynthAudioProcessor& audioProcessor;
    juce::ToggleButton deltaSizeLinkToggle, tooltipsToggle;
    
    float m_cloudLength;
    int m_envType = 1;

    void timerCallback() override;
    
    sjf_lookAndFeel otherLookandFeel;
    
    juce::String MAIN_TOOLTIP = "sjf_granSynth: \nGranular Cloud Generator\n\n1) load a sample \n2) click random cloud (you may have to start transport to get sound at first \n3) you can also try setting the parameters manually and then clicking trigger cloud"; 
    
//    juce::TooltipWindow tooltipWindow {this, 700};
//    juce::SharedResourcePointer<juce::TooltipWindow> tooltipWindow;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sjf_granSynthAudioProcessorEditor)
};
