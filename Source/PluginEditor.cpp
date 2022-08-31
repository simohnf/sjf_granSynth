/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Sjf_granSynthAudioProcessorEditor::Sjf_granSynthAudioProcessorEditor (Sjf_granSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
    addAndMakeVisible(&envTypeBox);
    envTypeBox.addItem("hann", 1);
    envTypeBox.addItem("triangle", 2);
    envTypeBox.addItem("sinc", 3);
    envTypeBox.addItem("expodec", 4);
    envTypeBox.addItem("rexpodec", 5);
    envTypeBox.onChange = [this]{ audioProcessor.m_grainEngine.setEnvType( envTypeBox.getSelectedId() ); };
    envTypeBox.setSelectedId(1);
    
    addAndMakeVisible(&loadSampleButton);
    loadSampleButton.setButtonText("load sample");
    loadSampleButton.onClick = [this]{ audioProcessor.m_grainEngine.loadSample() ; };
    

    
    addAndMakeVisible(&triggerRandomCloudButton);
    triggerRandomCloudButton.setButtonText("random cloud");
    triggerRandomCloudButton.onClick = [this]
    {
        grainPositionGraph.randomGraph();
        grainPanGraph.randomGraph();
        grainTransposeGraph.randomGraph();
        grainSizeGraph.randomGraph();
        grainGainGraph.randomGraph();
        grainDeltaGraph.randomGraph();
        audioProcessor.m_grainEngine.setCloudLength(500.0f + rand01()*29500.0f);
        auto envType = 1 + (int)( rand01() * envTypeBox.getNumItems() );
        audioProcessor.m_grainEngine.setEnvType( envType );
        envTypeBox.setSelectedId( envType );
        
        audioProcessor.m_grainEngine.setGrainPositionVector( grainPositionGraph.getGraphAsVector() );
        audioProcessor.m_grainEngine.setGrainPanVector( grainPanGraph.getGraphAsVector() );
        audioProcessor.m_grainEngine.setGrainTranspositionVector( grainTransposeGraph.getGraphAsVector() );
        audioProcessor.m_grainEngine.setGrainSizeVector( grainSizeGraph.getGraphAsVector() );
        audioProcessor.m_grainEngine.setGrainGainVector( grainGainGraph.getGraphAsVector() );
        audioProcessor.m_grainEngine.setGrainDeltaVector( grainDeltaGraph.getGraphAsVector() );
        audioProcessor.m_grainEngine.triggerNewCloud(true);
    };
//    triggerRandomCloudButton.onClick = [this]{ audioProcessor.m_grainEngine.newGrain(grainStartSlider.getValue(), grainLengthSlider.getValue(), grainTranspositionSlider.getValue(), 1.0f, grainPanSlider.getValue(), 0); };
//    audioProcessor.m_grainEngine.newGrain(float grainStart, float grainLengthMS, float transposition, float gain, float pan, int envType)
    
    
    
    addAndMakeVisible(&triggerCloudButton);
    triggerCloudButton.setButtonText("trigger cloud");
    triggerCloudButton.onClick = [this]
    {
//        grainPositionGraph.randomGraph();
//        grainPanGraph.randomGraph();
//        grainTransposeGraph.randomGraph();
//        grainSizeGraph.randomGraph();
//        grainGainGraph.randomGraph();
//        grainDeltaGraph.randomGraph();
        
        audioProcessor.m_grainEngine.setGrainPositionVector( grainPositionGraph.getGraphAsVector() );
        audioProcessor.m_grainEngine.setGrainPanVector( grainPanGraph.getGraphAsVector() );
        audioProcessor.m_grainEngine.setGrainTranspositionVector( grainTransposeGraph.getGraphAsVector() );
        audioProcessor.m_grainEngine.setGrainSizeVector( grainSizeGraph.getGraphAsVector() );
        audioProcessor.m_grainEngine.setGrainGainVector( grainGainGraph.getGraphAsVector() );
        audioProcessor.m_grainEngine.setGrainDeltaVector( grainDeltaGraph.getGraphAsVector() );
        audioProcessor.m_grainEngine.triggerNewCloud(true);
    };
    
    addAndMakeVisible(&randomGraphsButton);
    randomGraphsButton.setButtonText("randomise graphs");
    randomGraphsButton.onClick = [this]
    {
        grainPositionGraph.randomGraph();
        grainPanGraph.randomGraph();
        grainTransposeGraph.randomGraph();
        grainSizeGraph.randomGraph();
        grainGainGraph.randomGraph();
        grainDeltaGraph.randomGraph();
    };
    
    addAndMakeVisible(&grainLengthSlider);
    grainLengthSlider.setRange(1.0f, 100.0f);
    grainLengthSlider.setValue(100.0f);
    grainLengthSlider.onValueChange = [this]{ audioProcessor.m_grainEngine.setGrainLength( grainLengthSlider.getValue() ); } ;
    addAndMakeVisible(&lengthLabel);
    lengthLabel.attachToComponent(&grainLengthSlider, true);
    lengthLabel.setText("length", juce::dontSendNotification);
    
    addAndMakeVisible(&grainPanSlider);
    grainPanSlider.setRange(0.0f, 1.0f);
    grainPanSlider.setValue(0.5f);
    grainPanSlider.onValueChange = [this]{ audioProcessor.m_grainEngine.setPan( grainPanSlider.getValue() ); } ;
    addAndMakeVisible(&panLabel);
    panLabel.setText("pan", juce::dontSendNotification);
    panLabel.attachToComponent(&grainPanSlider, true);
    
    addAndMakeVisible(&grainStartSlider);
    grainStartSlider.setRange(0, 1);
    grainStartSlider.onValueChange = [this]{ audioProcessor.m_grainEngine.setGrainStart( grainStartSlider.getValue() ); };
    addAndMakeVisible(&startLabel);
    startLabel.setText("start", juce::dontSendNotification);
    startLabel.attachToComponent(&grainStartSlider, true);
    
    addAndMakeVisible(&grainTranspositionSlider);
    grainTranspositionSlider.setRange(-12, 12);
    grainTranspositionSlider.onValueChange = [this]{ audioProcessor.m_grainEngine.setTransposition( grainTranspositionSlider.getValue() ); };
    addAndMakeVisible(&transposeLabel);
    transposeLabel.setText("trans", juce::dontSendNotification);
    transposeLabel.attachToComponent(&grainTranspositionSlider, true);
    
   
    
    addAndMakeVisible(&grainPositionGraph);
    grainPositionGraph.setGraphText("grain start position");
    addAndMakeVisible(&grainPanGraph);
    grainPanGraph.setGraphText("grain pan");
    addAndMakeVisible(&grainTransposeGraph);
    grainTransposeGraph.setGraphText("grain transpose");
    addAndMakeVisible(&grainSizeGraph);
    grainSizeGraph.setGraphText("grain size");
    addAndMakeVisible(&grainGainGraph);
    grainGainGraph.setGraphText("grain gain");
    addAndMakeVisible(&grainDeltaGraph);
    grainDeltaGraph.setGraphText("grain delta time");
    
    setSize (1000, 700);
}

Sjf_granSynthAudioProcessorEditor::~Sjf_granSynthAudioProcessorEditor()
{
}

//==============================================================================
void Sjf_granSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void Sjf_granSynthAudioProcessorEditor::resized()
{
    loadSampleButton.setBounds(10, 10, 100, 20);
    randomGraphsButton.setBounds(110, 10, 100, 20);
    triggerRandomCloudButton.setBounds(10, 30, 100, 20);
    triggerCloudButton.setBounds(110, 30, 100, 20);
    
//    grainLengthSlider.setBounds(50, 50, 200, 20);
//    grainPanSlider.setBounds(50, 70, 200, 20);
//    grainStartSlider.setBounds(50, 90, 200, 20);
//    grainTranspositionSlider.setBounds(50, 110, 200, 20);
//
    
    
    grainDeltaGraph.setBounds(0, 50, 800, 100);
    grainPositionGraph.setBounds(0, 150, 800, 100);
    grainPanGraph.setBounds(0, 250, 800, 100);
    grainTransposeGraph.setBounds(0, 350, 800, 100);
    grainSizeGraph.setBounds(0, 450, 800, 100);
    grainGainGraph.setBounds(0, 550, 800, 100);
    
    envTypeBox.setBounds(0, 650, 150, 20);
    
}