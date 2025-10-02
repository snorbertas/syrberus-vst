/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters.h"

//==============================================================================
SyrberusAudioProcessor::SyrberusAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParams())
#endif
{
    synth.addSound(new SyrberusSound());

    for (int i = 0; i < 8; i++) {
        synth.addVoice(new SyrberusVoice());
    }
}

SyrberusAudioProcessor::~SyrberusAudioProcessor()
{
}

//==============================================================================
const juce::String SyrberusAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SyrberusAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SyrberusAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SyrberusAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SyrberusAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SyrberusAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SyrberusAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SyrberusAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SyrberusAudioProcessor::getProgramName (int index)
{
    return {};
}

void SyrberusAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SyrberusAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    synth.setCurrentPlaybackSampleRate(sampleRate);
    keyboardState.reset();

    for (int i = 0; i < synth.getNumVoices(); i++) {
        if (auto voice = dynamic_cast<SyrberusVoice*>(synth.getVoice(i)))
        {
            voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    limiter.prepare(spec);
    limiter.setThreshold(3.0f); // Example threshold in dB
    limiter.setRelease(10.0f);   // Release time in milliseconds
}

void SyrberusAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SyrberusAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void SyrberusAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, numSamples);

    float gain = apvts.getRawParameterValue(Params::miscGain)->load();
    float attack = apvts.getRawParameterValue(Params::envAttack)->load();
    float decay = apvts.getRawParameterValue(Params::envDecay)->load();
    float sustain = apvts.getRawParameterValue(Params::envSustain)->load();
    float release = apvts.getRawParameterValue(Params::envRelease)->load();

    int unisonVoices = apvts.getParameter(Params::unisonVoices)->getValue() * 16;
    float unisonDetune = apvts.getRawParameterValue(Params::unisonDetune)->load();


    UnisonVoice::SyrberusOscillatorParams syrOscParams(apvts);

    envelopeGraph.setParams(attack, decay, sustain, release);

    for (int i = 0; i < synth.getNumVoices(); i++) {
        if (auto voice = dynamic_cast<SyrberusVoice*>(synth.getVoice(i)))
        {
            voice->updateParams(gain, &envelopeGraph);
            voice->updateParams(syrOscParams);
            voice->setUnison(unisonVoices + 1, unisonDetune);
        }
    }

    keyboardState.processNextMidiBuffer(midiMessages, 0, numSamples, true);
    synth.renderNextBlock(buffer, midiMessages, 0, numSamples);

    juce::dsp::AudioBlock<float> audioBlock(buffer);
    limiter.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
}

//==============================================================================
bool SyrberusAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SyrberusAudioProcessor::createEditor()
{
    return new SyrberusAudioProcessorEditor (*this);
}

//==============================================================================
void SyrberusAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void SyrberusAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SyrberusAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout SyrberusAudioProcessor::createParams()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Envelope
    params.push_back(std::make_unique<juce::AudioParameterFloat>(Params::envAttack, "Attack", 0.0f, 5.0f, 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(Params::envHold, "Hold", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(Params::envDecay, "Decay", 0.0f, 5.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(Params::envSustain, "Sustain", 0.0f, 1.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(Params::envRelease, "Release", 0.0f, 5.0f, 0.5f));

    // Unison
    params.push_back(std::make_unique<juce::AudioParameterInt>(Params::unisonVoices, "Unison Voices", 0, 16, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(Params::unisonDetune, "Unison Detune", 0.0f, 1.0f, 0.07f));

    // Misc
    params.push_back(std::make_unique<juce::AudioParameterFloat>(Params::miscGain, "Gain", 0.0f, 1.0f, 0.5f));

    // Osc 1
    params.push_back(std::make_unique<juce::AudioParameterInt>(Params::osc1Shape, "Wave Shape (Osc1)", 0, 4, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(Params::osc1Phase, "Phase (Osc1)", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(Params::osc1Invert, "Invert (Osc1)", false));
    params.push_back(std::make_unique<juce::AudioParameterInt>(Params::osc1Transpose, "Transpose (Osc1)", -24, 24, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(Params::osc1Stereo, "Stereo (Osc1)", -1.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(Params::osc1Mix, "Mix (Osc1)", 0.0f, 1.0f, 1.0f));

    // Osc 2
    params.push_back(std::make_unique<juce::AudioParameterInt>(Params::osc2Shape, "Wave Shape (Osc2)", 0, 4, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(Params::osc2Phase, "Phase (Osc2)", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(Params::osc2Invert, "Invert (Osc2)", false));
    params.push_back(std::make_unique<juce::AudioParameterInt>(Params::osc2Transpose, "Transpose (Osc2)", -24, 24, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(Params::osc2Stereo, "Stereo (Osc2)", -1.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(Params::osc2Mix, "Mix (Osc2)", 0.0f, 1.0f, 1.0f));

    // Osc 3
    params.push_back(std::make_unique<juce::AudioParameterInt>(Params::osc3Shape, "Wave Shape (Osc3)", 0, 4, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(Params::osc3Phase, "Phase (Osc3)", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(Params::osc3Invert, "Invert (Osc3)", false));
    params.push_back(std::make_unique<juce::AudioParameterInt>(Params::osc3Transpose, "Transpose (Osc3)", -24, 24, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(Params::osc3Stereo, "Stereo (Osc3)", -1.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(Params::osc3Mix, "Mix (Osc3)", 0.0f, 1.0f, 1.0f));

    return { params.begin(), params.end() };
}