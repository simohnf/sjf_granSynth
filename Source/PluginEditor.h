/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "/Users/simonfay/Programming_Stuff/sjf_audio/sjf_lookAndFeel.h"
#include "/Users/simonfay/Programming_Stuff/sjf_audio/sjf_graph.h"
#include "/Users/simonfay/Programming_Stuff/sjf_audio/sjf_numBox.h"
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
    juce::Label revSizeLabel, revDampingLabel;
    juce::Slider reverbSizeSlider, reverbDampingSlider;
    Sjf_granSynthAudioProcessor& audioProcessor;
    juce::ToggleButton deltaSizeLinkToggle;
    
    float m_cloudLength;

    void timerCallback() override;
    
    sjf_lookAndFeel otherLookandFeel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sjf_granSynthAudioProcessorEditor)
};
