/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "/Users/simonfay/JUCE_PROJECTS/sjf_audio/sjf_lookAndFeel.h"
#include "/Users/simonfay/JUCE_PROJECTS/sjf_audio/sjf_graph.h"
#include "/Users/simonfay/JUCE_PROJECTS/sjf_audio/sjf_numBox.h"
//==============================================================================
/**
*/
class Sjf_granSynthAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Sjf_granSynthAudioProcessorEditor (Sjf_granSynthAudioProcessor&);
    ~Sjf_granSynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void randomiseGraphs();
    void getGraphsAsVectors();

private:
    
    juce::TextButton loadSampleButton, triggerRandomCloudButton, triggerCloudButton, randomGraphsButton;
    juce::ComboBox envTypeBox;
    juce::Label startLabel, panLabel, transposeLabel, lengthLabel;
    juce::Slider grainStartSlider, grainPanSlider, grainTranspositionSlider, grainLengthSlider;
    sjf_numBox cloudLengthNumBox;
    sjf_grapher grainPositionGraph, grainPanGraph, grainTransposeGraph, grainSizeGraph, grainGainGraph, grainDeltaGraph, grainReverbGraph;
    
    Sjf_granSynthAudioProcessor& audioProcessor;

    sjf_lookAndFeel otherLookandFeel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sjf_granSynthAudioProcessorEditor)
};
