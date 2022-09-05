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
        randomiseVariables();
        triggerNewCloud();
    };
    
    addAndMakeVisible(&triggerCloudButton);
    triggerCloudButton.setButtonText("trigger cloud");
    triggerCloudButton.onClick = [this]
    {
        triggerNewCloud();
    };
    
    addAndMakeVisible(&randomGraphsButton);
    randomGraphsButton.setButtonText("randomise graphs");
    randomGraphsButton.onClick = [this]
    {
        randomiseGraphs();
    };
    
    
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
    cloudLengthNumBox.onValueChange = [this]{ m_cloudLength = cloudLengthNumBox.getValue(); };
    
    
    addAndMakeVisible( &reverbSizeSlider );
    reverbSizeSlider.setRange(0, 1);
    reverbSizeSlider.setValue(0.5);
    reverbSizeSlider.setSliderStyle( juce::Slider::Rotary );
    addAndMakeVisible( &revSizeLabel );
    revSizeLabel.attachToComponent( &reverbSizeSlider, false );
    revSizeLabel.setText("size", juce::dontSendNotification);
    revSizeLabel.setJustificationType( juce::Justification::centred );
    
    addAndMakeVisible( &reverbDampingSlider );
    reverbDampingSlider.setRange(0, 1);
    reverbDampingSlider.setValue(0.5);
    reverbDampingSlider.setSliderStyle( juce::Slider::Rotary );
    addAndMakeVisible( &revDampingLabel );
    revDampingLabel.attachToComponent( &reverbDampingSlider, false );
    revDampingLabel.setText("damp", juce::dontSendNotification);
    revDampingLabel.setJustificationType( juce::Justification::centred );
    
    
    addAndMakeVisible( &deltaSizeLinkToggle );
    deltaSizeLinkToggle.setButtonText("link delta time and grain size");
    deltaSizeLinkToggle.onStateChange = [this]{ audioProcessor.m_grainEngine.linkSizeAndDeltaTime( deltaSizeLinkToggle.getToggleState() ); };
    
    startTimer(100);
    
    setSize (600, 500);
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
//    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
    g.drawFittedText("sjf_granSynth", 0, 0, getWidth(), 20, juce::Justification::centred, 1);

    
}

void Sjf_granSynthAudioProcessorEditor::resized()
{
    auto graphWidth =  (float)getWidth() * 0.8f;
    auto graphHeight = ( (float)getHeight() -20.0f ) / 7.0f ;
    auto buttonWidth = 0.8f * (getWidth() - graphWidth);
    auto buttonHeight = 20.0f;
    auto gap = 0.1f * (getWidth() - graphWidth);
    loadSampleButton.setBounds( gap+graphWidth, 20, buttonWidth, buttonHeight);
    randomGraphsButton.setBounds( gap+graphWidth, buttonHeight*3, buttonWidth, buttonHeight);
    triggerRandomCloudButton.setBounds( gap+graphWidth, buttonHeight*4, buttonWidth, buttonHeight*4);
    triggerCloudButton.setBounds( gap+graphWidth, buttonHeight*9, buttonWidth, buttonHeight*4);
    envTypeBox.setBounds( gap+graphWidth, buttonHeight*13, buttonWidth, buttonHeight);
    cloudLengthNumBox.setBounds( gap+graphWidth, buttonHeight*14, buttonWidth, buttonHeight);
    deltaSizeLinkToggle.setBounds(gap+graphWidth, buttonHeight*16, buttonWidth, buttonHeight*4);
    
    
    grainDeltaGraph.setBounds(0, 20, graphWidth, graphHeight);
    grainPositionGraph.setBounds(0, 20 + graphHeight, graphWidth, graphHeight);
    grainPanGraph.setBounds(0, 20 + graphHeight*2, graphWidth, graphHeight);
    grainTransposeGraph.setBounds(0,20 +  graphHeight*3, graphWidth, graphHeight);
    grainSizeGraph.setBounds(0, 20 + graphHeight*4, graphWidth, graphHeight);
    grainGainGraph.setBounds(0, 20 + graphHeight*5, graphWidth, graphHeight);
    grainReverbGraph.setBounds(0, 20 +graphHeight*6, graphWidth, graphHeight);
    
    reverbSizeSlider.setBounds(grainReverbGraph.getX() + graphWidth, grainReverbGraph.getY(), (getWidth() - graphWidth) * 0.5f, graphHeight);
    reverbSizeSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, reverbSizeSlider.getWidth(), buttonHeight);
    reverbDampingSlider.setBounds(reverbSizeSlider.getX() + reverbSizeSlider.getWidth(), reverbSizeSlider.getY(), reverbSizeSlider.getWidth(), graphHeight);
    reverbDampingSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, reverbDampingSlider.getWidth(), buttonHeight);
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


void Sjf_granSynthAudioProcessorEditor::timerCallback()
{
    auto cloudPhase = audioProcessor.m_grainEngine.getCurrentCloudPhase();
    grainPositionGraph.setGraphPosition( cloudPhase );
    grainPanGraph.setGraphPosition( cloudPhase );
    grainTransposeGraph.setGraphPosition( cloudPhase );
    grainSizeGraph.setGraphPosition( cloudPhase );
    grainGainGraph.setGraphPosition( cloudPhase );
    grainDeltaGraph.setGraphPosition( cloudPhase );
    grainReverbGraph.setGraphPosition( cloudPhase );
}

void Sjf_granSynthAudioProcessorEditor::triggerNewCloud()
{
    getGraphsAsVectors();
    audioProcessor.m_grainEngine.setCloudLength( m_cloudLength );
    audioProcessor.m_grainEngine.setReverbSize( reverbSizeSlider.getValue() );
    audioProcessor.m_grainEngine.setReverbDamping( reverbDampingSlider.getValue() );
    audioProcessor.m_grainEngine.triggerNewCloud(true);
}


void Sjf_granSynthAudioProcessorEditor::randomiseVariables()
{
    m_cloudLength = 500.0f + rand01()*29500.0f;
    audioProcessor.m_grainEngine.setCloudLength( m_cloudLength );
    cloudLengthNumBox.setValue( m_cloudLength, juce::dontSendNotification );
    
    auto envType = 1 + (int)( rand01() * envTypeBox.getNumItems() );
    audioProcessor.m_grainEngine.setEnvType( envType );
    envTypeBox.setSelectedId( envType, juce::dontSendNotification );
}
