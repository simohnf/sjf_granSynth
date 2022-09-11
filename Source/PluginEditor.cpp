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
    getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::white.withAlpha(0.7f));
    
    addAndMakeVisible(&envTypeBox);
    envTypeBox.addItem("hann", 1);
    envTypeBox.addItem("triangle", 2);
    envTypeBox.addItem("sinc", 3);
    envTypeBox.addItem("expodec", 4);
    envTypeBox.addItem("rexpodec", 5);
    envTypeBox.onChange = [this]{ m_envType = envTypeBox.getSelectedId(); };
    envTypeBox.setSelectedId( audioProcessor.m_grainEngine.getEnvType() );
    envTypeBox.setTooltip("This determines the envelope/window to be applied to each grain.");
    
    addAndMakeVisible(&loadSampleButton);
    loadSampleButton.setButtonText("load sample");
    loadSampleButton.onClick = [this]{ audioProcessor.m_grainEngine.loadSample() ; };
    loadSampleButton.setTooltip("Use this to load a .wav/.aif audio sample to granulate.");

    
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
    grainPositionGraph.setTopLabel("End of file");
    grainPositionGraph.setBottomLabel("Beginning of file");
    grainPositionGraph.setTooltip("This determines the starting point of each grain in relation to the total length of the audio file");
    
    addAndMakeVisible(&grainPanGraph);
    grainPanGraph.setGraphText("grain pan");
    if ( audioProcessor.m_grainEngine.getGrainPanVector().size() > 0 )
    {
        grainPanGraph.setGraph( audioProcessor.m_grainEngine.getGrainPanVector() );
    }
    grainPanGraph.setTooltip("This determines the panning for each grain");
    grainPanGraph.setTopLabel("Right");
    grainPanGraph.setBottomLabel("Left");
    
    addAndMakeVisible(&grainTransposeGraph);
    grainTransposeGraph.setGraphText("grain transpose");
    if ( audioProcessor.m_grainEngine.getGrainTranspositionVector().size() > 0 )
    {
        grainTransposeGraph.setGraph( audioProcessor.m_grainEngine.getGrainTranspositionVector() );
    }
    grainTransposeGraph.setTooltip("This determines the pitch shifting applied to each grain (a maximum of one octave up or down)");
    
    addAndMakeVisible(&grainSizeGraph);
    grainSizeGraph.setGraphText("grain size");
    if ( audioProcessor.m_grainEngine.getGrainSizeVector().size() > 0 )
    {
        grainSizeGraph.setGraph( audioProcessor.m_grainEngine.getGrainSizeVector() );
    }
    grainSizeGraph.setTooltip("This determines the length of each grain");
    grainSizeGraph.setTopLabel("100ms");
    grainSizeGraph.setBottomLabel("1ms");
    
    addAndMakeVisible(&grainGainGraph);
    grainGainGraph.setGraphText("grain gain");
    if ( audioProcessor.m_grainEngine.getGrainGainVector().size() > 0 )
    {
        grainGainGraph.setGraph( audioProcessor.m_grainEngine.getGrainGainVector() );
    }
    grainGainGraph.setTooltip("This determines the gain to be applied to each grain");
    grainGainGraph.setTopLabel("100%");
    grainGainGraph.setBottomLabel("0%");
    
    addAndMakeVisible(&grainDeltaGraph);
    grainDeltaGraph.setGraphText("grain delta time");
    if ( audioProcessor.m_grainEngine.getGrainDeltaVector().size() > 0 )
    {
        grainDeltaGraph.setGraph( audioProcessor.m_grainEngine.getGrainDeltaVector() );
    }
    grainDeltaGraph.setTooltip("This determines the time between each grain");
    grainDeltaGraph.setTopLabel("100ms");
    grainDeltaGraph.setBottomLabel("1ms");
    
    addAndMakeVisible(&grainReverbGraph);
    grainReverbGraph.setGraphText("grain reverb");
    if ( audioProcessor.m_grainEngine.getGrainReverbVector().size() > 0 )
    {
        grainReverbGraph.setGraph( audioProcessor.m_grainEngine.getGrainReverbVector() );
    }
    grainReverbGraph.setTooltip("This determines the amount of reverb for each grain");
    grainReverbGraph.setTopLabel("Wet");
    grainReverbGraph.setBottomLabel("Dry");
    
    addAndMakeVisible( &cloudLengthNumBox );
    cloudLengthNumBox.setRange( 500.0f, 60000.0f, 1.0f );
    cloudLengthNumBox.setTextValueSuffix(" ms");
    cloudLengthNumBox.setValue( audioProcessor.m_grainEngine.getCloudLengthMS() );
    cloudLengthNumBox.onValueChange = [this]{ m_cloudLength = cloudLengthNumBox.getValue(); };
    cloudLengthNumBox.setTooltip("This determines the length of the cloud to be generated (not including any reverb tail)");
    cloudLengthNumBox.sendLookAndFeelChange();
    
    
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
    deltaSizeLinkToggle.onStateChange = [this]
    {
        audioProcessor.m_grainEngine.linkSizeAndDeltaTime( deltaSizeLinkToggle.getToggleState() );
        if (deltaSizeLinkToggle.getToggleState())
        {
            grainSizeGraph.setTopLabel("Delta * 10");
            grainSizeGraph.setBottomLabel("Delta * 1");
        }
        else
        {
            grainSizeGraph.setTopLabel("100ms");
            grainSizeGraph.setBottomLabel("1ms");
        }
    };
    deltaSizeLinkToggle.setTooltip("If the deltatime (time between grains) is very short, and grain size (length of grains) is long cpu usage can be very high - causing audio glitches. Linking both parameters means that the grain size will be limited to a maximum of 10 times the delta time. This should reduce any audio glitches... hopefully!");
    
    
    addAndMakeVisible(&tooltipsToggle);
    tooltipsToggle.setButtonText("Hints");
    tooltipsToggle.onStateChange = [this]
    {
        if (tooltipsToggle.getToggleState())
        {
            tooltipWindow.getObject().setAlpha(1.0f);
        }
        else
        {
            tooltipWindow.getObject().setAlpha(0.0f);
        }
    };
    tooltipWindow.getObject().setAlpha(0.0f);
    
    addAndMakeVisible(&sampleNameLabel);
    
    startTimer(100);
    
    setSize (620, 500);
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
    auto offset = 20.0f;
    auto graphWidth =  (float)getWidth() * 0.8f;
    auto graphHeight = ( (float)getHeight() -40.0f ) / 7.0f ;
    auto buttonWidth = 0.8f * (getWidth() - graphWidth);
    auto buttonHeight = offset;
    auto gap = 0.1f * (getWidth() - graphWidth);
    loadSampleButton.setBounds( gap+graphWidth, 20, buttonWidth, buttonHeight);
    randomGraphsButton.setBounds( gap+graphWidth, buttonHeight*3, buttonWidth, buttonHeight);
    triggerRandomCloudButton.setBounds( gap+graphWidth, buttonHeight*4, buttonWidth, buttonHeight*4);
    triggerCloudButton.setBounds( gap+graphWidth, buttonHeight*9, buttonWidth, buttonHeight*4);
    envTypeBox.setBounds( gap+graphWidth, buttonHeight*13, buttonWidth, buttonHeight);
    cloudLengthNumBox.setBounds( gap+graphWidth, buttonHeight*14, buttonWidth, buttonHeight);
    deltaSizeLinkToggle.setBounds(gap+graphWidth, buttonHeight*16, buttonWidth, buttonHeight*3);
    
    
    grainDeltaGraph.setBounds(0, offset, graphWidth, graphHeight);
    grainPositionGraph.setBounds(0, offset + graphHeight, graphWidth, graphHeight);
    grainPanGraph.setBounds(0, offset + graphHeight*2, graphWidth, graphHeight);
    grainTransposeGraph.setBounds(0,offset +  graphHeight*3, graphWidth, graphHeight);
    grainSizeGraph.setBounds(0, offset + graphHeight*4, graphWidth, graphHeight);
    grainGainGraph.setBounds(0, offset + graphHeight*5, graphWidth, graphHeight);
    grainReverbGraph.setBounds(0, offset +graphHeight*6, graphWidth, graphHeight);
    
    reverbSizeSlider.setBounds(grainReverbGraph.getX() + graphWidth, grainReverbGraph.getY(), (getWidth() - graphWidth) * 0.5f, graphHeight);
    reverbSizeSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, reverbSizeSlider.getWidth(), buttonHeight);
    reverbDampingSlider.setBounds(reverbSizeSlider.getX() + reverbSizeSlider.getWidth(), reverbSizeSlider.getY(), reverbSizeSlider.getWidth(), graphHeight);
    reverbDampingSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, reverbDampingSlider.getWidth(), buttonHeight);
    

    tooltipsToggle.setBounds(0, getHeight() - offset, graphWidth * 0.15f, buttonHeight);
    
    sampleNameLabel.setBounds(tooltipsToggle.getWidth(), tooltipsToggle.getY(), getWidth() - tooltipsToggle.getWidth(), buttonHeight);
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
    
    sampleNameLabel.setText(audioProcessor.m_grainEngine.m_samplePath.getFileName(), juce::dontSendNotification);
    sampleNameLabel.setJustificationType(juce::Justification::centred);
    
    cloudLengthNumBox.setRange( 500.0f, fmax( 60000.0f, audioProcessor.m_grainEngine.getDurationMS() ), 1.0f );
}

void Sjf_granSynthAudioProcessorEditor::triggerNewCloud()
{
    getGraphsAsVectors();
    audioProcessor.m_grainEngine.setEnvType( m_envType );
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



//==============================================================================



