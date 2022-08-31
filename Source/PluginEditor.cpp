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
    setLookAndFeel( &otherLookandFeel );
    
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
        randomiseGraphs();
        
        auto newCloudLength = 500.0f + rand01()*29500.0f;
        audioProcessor.m_grainEngine.setCloudLength( newCloudLength );
        cloudLengthNumBox.setValue( newCloudLength, juce::dontSendNotification );
        
        auto envType = 1 + (int)( rand01() * envTypeBox.getNumItems() );
        audioProcessor.m_grainEngine.setEnvType( envType );
        envTypeBox.setSelectedId( envType, juce::dontSendNotification );
        
        getGraphsAsVectors();
        audioProcessor.m_grainEngine.triggerNewCloud(true);
    };
    
    addAndMakeVisible(&triggerCloudButton);
    triggerCloudButton.setButtonText("trigger cloud");
    triggerCloudButton.onClick = [this]
    {
        getGraphsAsVectors();
        audioProcessor.m_grainEngine.triggerNewCloud(true);
    };
    
    addAndMakeVisible(&randomGraphsButton);
    randomGraphsButton.setButtonText("randomise graphs");
    randomGraphsButton.onClick = [this]
    {
        randomiseGraphs();
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
    addAndMakeVisible(&grainReverbGraph);
    grainReverbGraph.setGraphText("grain reverb");
    
    addAndMakeVisible( &cloudLengthNumBox );
    cloudLengthNumBox.setRange( 500.0f, 60000.0f, 1.0f );
    cloudLengthNumBox.setTextValueSuffix(" ms");
    cloudLengthNumBox.setValue( audioProcessor.m_grainEngine.getCloudLengthMS() );
    cloudLengthNumBox.onValueChange = [this]{ audioProcessor.m_grainEngine.setCloudLength( cloudLengthNumBox.getValue() ); };
    
    
    setSize (1000, 700);
}

Sjf_granSynthAudioProcessorEditor::~Sjf_granSynthAudioProcessorEditor()
{
    setLookAndFeel( nullptr );
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
    loadSampleButton.setBounds(800, 0, 100, 20);
    randomGraphsButton.setBounds(800, 20, 100, 20);
    triggerRandomCloudButton.setBounds(800, 40, 100, 20);
    triggerCloudButton.setBounds(800, 60, 100, 20);
    envTypeBox.setBounds(800, 80, 100, 20);
    cloudLengthNumBox.setBounds(800, 100, 100, 20);
//    grainLengthSlider.setBounds(50, 50, 200, 20);
//    grainPanSlider.setBounds(50, 70, 200, 20);
//    grainStartSlider.setBounds(50, 90, 200, 20);
//    grainTranspositionSlider.setBounds(50, 110, 200, 20);
//
    
    
    grainDeltaGraph.setBounds(0, 0, 800, 100);
    grainPositionGraph.setBounds(0, 100, 800, 100);
    grainPanGraph.setBounds(0, 200, 800, 100);
    grainTransposeGraph.setBounds(0, 300, 800, 100);
    grainSizeGraph.setBounds(0, 400, 800, 100);
    grainGainGraph.setBounds(0, 500, 800, 100);
    grainReverbGraph.setBounds(0, 600, 800, 100);
    
    
    
}

void Sjf_granSynthAudioProcessorEditor::randomiseGraphs()
{
    grainPositionGraph.randomGraph();
    grainPanGraph.randomGraph();
    grainTransposeGraph.randomGraph();
    grainSizeGraph.randomGraph();
    grainGainGraph.randomGraph();
    grainDeltaGraph.randomGraph();
    grainReverbGraph.randomGraph();
}

void Sjf_granSynthAudioProcessorEditor::getGraphsAsVectors()
{
    audioProcessor.m_grainEngine.setGrainPositionVector( grainPositionGraph.getGraphAsVector() );
    audioProcessor.m_grainEngine.setGrainPanVector( grainPanGraph.getGraphAsVector() );
    audioProcessor.m_grainEngine.setGrainTranspositionVector( grainTransposeGraph.getGraphAsVector() );
    audioProcessor.m_grainEngine.setGrainSizeVector( grainSizeGraph.getGraphAsVector() );
    audioProcessor.m_grainEngine.setGrainGainVector( grainGainGraph.getGraphAsVector() );
    audioProcessor.m_grainEngine.setGrainDeltaVector( grainDeltaGraph.getGraphAsVector() );
    audioProcessor.m_grainEngine.setGrainReverbVector( grainReverbGraph.getGraphAsVector() );
    
}
