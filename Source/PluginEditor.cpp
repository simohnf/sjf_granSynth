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
//    setLookAndFeel( &otherLookandFeel );
    
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
    grainLengthSlider.setSliderStyle( juce::Slider::TwoValueVertical );
    grainLengthSlider.setMinValue( 1.0f );
    grainLengthSlider.setMaxValue( 100.0f );
//    grainLengthSlider.setValue(1.0f, 100.0f);
//    grainLengthSlider.onValueChange = [this]{ audioProcessor.m_grainEngine.setGrainLength( grainLengthSlider.getValue() ); } ;
//    addAndMakeVisible(&lengthLabel);
//    lengthLabel.attachToComponent(&grainLengthSlider, false);
//    lengthLabel.setText("length", juce::dontSendNotification);
    
    addAndMakeVisible(&grainPanSlider);
    grainPanSlider.setRange(0.0f, 1.0f);
    grainPanSlider.setSliderStyle( juce::Slider::TwoValueVertical );
    grainPanSlider.setMinValue( 0.0f );
    grainPanSlider.setMaxValue( 1.0f );
//    grainPanSlider.setValue(0.5f);
//    grainPanSlider.onValueChange = [this]{ audioProcessor.m_grainEngine.setPan( grainPanSlider.getValue() ); } ;
//    addAndMakeVisible(&panLabel);
//    panLabel.setText("pan", juce::dontSendNotification);
//    panLabel.attachToComponent(&grainPanSlider, true);
    
    addAndMakeVisible(&grainStartSlider);
    grainStartSlider.setRange(0, 1);
    grainStartSlider.setSliderStyle( juce::Slider::TwoValueVertical );
    grainStartSlider.setMinValue( 0.0f );
    grainStartSlider.setMaxValue( 1.0f );
//    grainStartSlider.onValueChange = [this]{ audioProcessor.m_grainEngine.setGrainStart( grainStartSlider.getValue() ); };
//    addAndMakeVisible(&startLabel);
//    startLabel.setText("start", juce::dontSendNotification);
//    startLabel.attachToComponent(&grainStartSlider, true);
    
    addAndMakeVisible(&grainTranspositionSlider);
    grainTranspositionSlider.setRange(-12, 12);
    grainTranspositionSlider.setSliderStyle( juce::Slider::TwoValueVertical );
    grainTranspositionSlider.setMinValue( -12.0f );
    grainTranspositionSlider.setMaxValue( 12.0f );
//    grainTranspositionSlider.onValueChange = [this]{ audioProcessor.m_grainEngine.setTransposition( grainTranspositionSlider.getValue() ); };
//    addAndMakeVisible(&transposeLabel);
//    transposeLabel.setText("trans", juce::dontSendNotification);
//    transposeLabel.attachToComponent(&grainTranspositionSlider, true);
    
   
    
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
    auto graphWidth = 800;
    auto graphHeight = 100;
    loadSampleButton.setBounds(graphWidth+50, 0, 100, 20);
    randomGraphsButton.setBounds(graphWidth+50, 20, 100, 20);
    triggerRandomCloudButton.setBounds(graphWidth+50, 40, 100, 20);
    triggerCloudButton.setBounds(graphWidth+50, 60, 100, 20);
    envTypeBox.setBounds(graphWidth+50, 80, 100, 20);
    cloudLengthNumBox.setBounds(graphWidth+50, 100, 100, 20);
    
    
    
    grainDeltaGraph.setBounds(0, 0, graphWidth, graphHeight);
    grainPositionGraph.setBounds(0, 100, graphWidth, graphHeight);
    grainStartSlider.setBounds(grainPositionGraph.getX() + graphWidth, grainPositionGraph.getY(), 20, graphHeight);
    
    grainPanGraph.setBounds(0, 200, graphWidth, graphHeight);
    grainPanSlider.setBounds(grainPanGraph.getX() + graphWidth, grainPanGraph.getY(), 20, graphHeight);
    
    grainTransposeGraph.setBounds(0, 300, graphWidth, graphHeight);
    grainTranspositionSlider.setBounds(grainTransposeGraph.getX() + graphWidth, grainTransposeGraph.getY(), 20, graphHeight);
    
    grainSizeGraph.setBounds(0, 400, graphWidth, graphHeight);
    grainLengthSlider.setBounds(grainSizeGraph.getX() + graphWidth, grainSizeGraph.getY(), 20, graphHeight);
    
    grainGainGraph.setBounds(0, 500, graphWidth, graphHeight);
    grainReverbGraph.setBounds(0, 600, graphWidth, graphHeight);
    
    
    
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
