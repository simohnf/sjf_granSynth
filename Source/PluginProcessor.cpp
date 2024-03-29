/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Sjf_granSynthAudioProcessor::Sjf_granSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
, parameters(*this, nullptr, juce::Identifier("sjf_granSynth"),
            {

            })

{
    
    filePathParameter = parameters.state.getPropertyAsValue("sampleFilePath", nullptr, true);
    
    m_grainEngine.initialise(getSampleRate());
}

Sjf_granSynthAudioProcessor::~Sjf_granSynthAudioProcessor()
{
}

//==============================================================================
const juce::String Sjf_granSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Sjf_granSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Sjf_granSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Sjf_granSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Sjf_granSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Sjf_granSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Sjf_granSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Sjf_granSynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Sjf_granSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void Sjf_granSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Sjf_granSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    m_grainEngine.initialiseGranSynth( sampleRate, samplesPerBlock );
}

void Sjf_granSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Sjf_granSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Sjf_granSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
//    auto totalNumInputChannels  = getTotalNumInputChannels();
//    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    buffer.clear();
//    m_grainEngine.playGrains(buffer);
//    m_grainEngine.playCloud(buffer);
    m_grainEngine.playCloudFromVectors( buffer );
}

//==============================================================================
bool Sjf_granSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Sjf_granSynthAudioProcessor::createEditor()
{
    return new Sjf_granSynthAudioProcessorEditor (*this);
}

//==============================================================================
void Sjf_granSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    filePathParameter.setValue( m_grainEngine.getFilePath() );
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void Sjf_granSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType())){
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
            filePathParameter.referTo(parameters.state.getPropertyAsValue("sampleFilePath", nullptr) );
            if (filePathParameter != juce::Value{}){ m_grainEngine.loadSample( filePathParameter ); }
            
            
        }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Sjf_granSynthAudioProcessor();
}
