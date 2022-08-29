/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "/Users/simonfay/Programming_Stuff/sjf_audio/sjf_lookAndFeel.h"
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
    
    juce::TextButton loadSampleButton, triggerGrainButton, triggerCloudButton;
    juce::ComboBox envTypeBox;
    juce::Label startLabel, panLabel, transposeLabel, lengthLabel;
    juce::Slider grainStartSlider, grainPanSlider, grainTranspositionSlider, grainLengthSlider;
    
    Sjf_granSynthAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sjf_granSynthAudioProcessorEditor)
};
