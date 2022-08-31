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

private:
    
    juce::TextButton loadSampleButton, triggerRandomCloudButton, triggerCloudButton, randomGraphsButton;
    juce::ComboBox envTypeBox;
    juce::Label startLabel, panLabel, transposeLabel, lengthLabel;
    juce::Slider grainStartSlider, grainPanSlider, grainTranspositionSlider, grainLengthSlider;
    sjf_grapher grainPositionGraph, grainPanGraph, grainTransposeGraph, grainSizeGraph, grainGainGraph, grainDeltaGraph;
    
    Sjf_granSynthAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sjf_granSynthAudioProcessorEditor)
};