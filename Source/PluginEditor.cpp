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
    envTypeBox.setSelectedId( audioProcessor.m_grainEngine.getEnvType() );
    envTypeBox.setTooltip("This determines the envelope/window to be applied to each grain.");
    
    addAndMakeVisible(&loadSampleButton);
    loadSampleButton.setButtonText("load sample");
    loadSampleButton.onClick = [this]{ audioProcessor.m_grainEngine.loadSample() ; };
    loadSampleButton.setTooltip("Use this to load a .wav audio sample to granulate.");

    
    addAndMakeVisible(&triggerRandomCloudButton);
    triggerRandomCloudButton.setButtonText("random cloud");
    triggerRandomCloudButton.onClick = [this]
    {
        randomiseGraphs();
        randomiseVariables();
        triggerNewCloud();
    };
    triggerRandomCloudButton.setTooltip("This will trigger a new, random cloud.");
    
    addAndMakeVisible(&triggerCloudButton);
    triggerCloudButton.setButtonText("trigger cloud");
    triggerCloudButton.onClick = [this]
    {
        triggerNewCloud();
    };
    triggerCloudButton.setTooltip("This triggers a cloud using all of the currently selected parameters.");
    
    addAndMakeVisible(&randomGraphsButton);
    randomGraphsButton.setButtonText("randomise graphs");
    randomGraphsButton.onClick = [this]
    {
        randomiseGraphs();
    };
    randomGraphsButton.setTooltip("This will randomise the graphs, but will not trigger a cloud.");

    addAndMakeVisible(&grainPositionGraph);
    grainPositionGraph.setGraphText("grain start position");
    if ( audioProcessor.m_grainEngine.getGrainPositionVector().size() > 0 )
    {
        grainPositionGraph.setGraph( audioProcessor.m_grainEngine.getGrainPositionVector() );
    }
    
    addAndMakeVisible(&grainPanGraph);
    grainPanGraph.setGraphText("grain pan");
    if ( audioProcessor.m_grainEngine.getGrainPanVector().size() > 0 )
    {
        grainPanGraph.setGraph( audioProcessor.m_grainEngine.getGrainPanVector() );
    }
    
    addAndMakeVisible(&grainTransposeGraph);
    grainTransposeGraph.setGraphText("grain transpose");
    if ( audioProcessor.m_grainEngine.getGrainTranspositionVector().size() > 0 )
    {
        grainTransposeGraph.setGraph( audioProcessor.m_grainEngine.getGrainTranspositionVector() );
    }
    
    addAndMakeVisible(&grainSizeGraph);
    grainSizeGraph.setGraphText("grain size");
    if ( audioProcessor.m_grainEngine.getGrainSizeVector().size() > 0 )
    {
        grainSizeGraph.setGraph( audioProcessor.m_grainEngine.getGrainSizeVector() );
    }
    
    addAndMakeVisible(&grainGainGraph);
    grainGainGraph.setGraphText("grain gain");
    if ( audioProcessor.m_grainEngine.getGrainGainVector().size() > 0 )
    {
        grainGainGraph.setGraph( audioProcessor.m_grainEngine.getGrainGainVector() );
    }
    
    addAndMakeVisible(&grainDeltaGraph);
    grainDeltaGraph.setGraphText("grain delta time");
    if ( audioProcessor.m_grainEngine.getGrainDeltaVector().size() > 0 )
    {
        grainDeltaGraph.setGraph( audioProcessor.m_grainEngine.getGrainDeltaVector() );
    }
    
    addAndMakeVisible(&grainReverbGraph);
    grainReverbGraph.setGraphText("grain reverb");
    if ( audioProcessor.m_grainEngine.getGrainReverbVector().size() > 0 )
    {
        grainReverbGraph.setGraph( audioProcessor.m_grainEngine.getGrainReverbVector() );
    }
    
    
    addAndMakeVisible( &cloudLengthNumBox );
    cloudLengthNumBox.setRange( 500.0f, 60000.0f, 1.0f );
    cloudLengthNumBox.setTextValueSuffix(" ms");
    cloudLengthNumBox.setValue( audioProcessor.m_grainEngine.getCloudLengthMS() );
    cloudLengthNumBox.onValueChange = [this]{ m_cloudLength = cloudLengthNumBox.getValue(); };
    cloudLengthNumBox.setTooltip("This determines the length of the cloud to be generated (not including any reverb tail)");
    
    
    addAndMakeVisible( &reverbSizeSlider );
    reverbSizeSlider.setRange(0, 1);
    reverbSizeSlider.setValue( audioProcessor.m_grainEngine.getReverbSize() );
    reverbSizeSlider.setSliderStyle( juce::Slider::Rotary );
    addAndMakeVisible( &revSizeLabel );
    revSizeLabel.attachToComponent( &reverbSizeSlider, false );
    revSizeLabel.setText("size", juce::dontSendNotification);
    revSizeLabel.setJustificationType( juce::Justification::centred );
    reverbSizeSlider.setTooltip("This sets the room size of the reverb");
    
    addAndMakeVisible( &reverbDampingSlider );
    reverbDampingSlider.setRange(0, 1);
    reverbDampingSlider.setValue( audioProcessor.m_grainEngine.getReverbDamping() );
    reverbDampingSlider.setSliderStyle( juce::Slider::Rotary );
    addAndMakeVisible( &revDampingLabel );
    revDampingLabel.attachToComponent( &reverbDampingSlider, false );
    revDampingLabel.setText("damp", juce::dontSendNotification);
    revDampingLabel.setJustificationType( juce::Justification::centred );
    reverbDampingSlider.setTooltip("This sets the amount of high frequency damping applied to the reverb");
    
    addAndMakeVisible( &deltaSizeLinkToggle );
    deltaSizeLinkToggle.setButtonText("link delta time and grain size");
    deltaSizeLinkToggle.onStateChange = [this]{ audioProcessor.m_grainEngine.linkSizeAndDeltaTime( deltaSizeLinkToggle.getToggleState() ); };
    deltaSizeLinkToggle.setTooltip("If the deltatime (time between grains) is very short, and grain size (length of grains) are is long cpu usage can be very high - causing audio glitches. Linking both parameters means that the grain size will be limited to a maximum of 10 times the delta time. This should reduce any audio glitches... hopefully!");
    
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
    
    auto revDamp = rand01();
    audioProcessor.m_grainEngine.setReverbDamping( revDamp );
    reverbDampingSlider.setValue( revDamp, juce::dontSendNotification );
    
    auto revSize = rand01();
    audioProcessor.m_grainEngine.setReverbSize( revSize );
    reverbSizeSlider.setValue( revSize, juce::dontSendNotification );
    
    auto envType = 1 + (int)( rand01() * envTypeBox.getNumItems() );
    audioProcessor.m_grainEngine.setEnvType( envType );
    envTypeBox.setSelectedId( envType, juce::dontSendNotification );
}
