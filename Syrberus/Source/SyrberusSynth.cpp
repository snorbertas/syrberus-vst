/*
  ==============================================================================

    SyrberusSynth.cpp
    Created: 11 Feb 2025 11:19:50pm
    Author:  Norb

  ==============================================================================
*/

#include "SyrberusSynth.h"

bool SyrberusSound::appliesToNote(int) { return true; }
bool SyrberusSound::appliesToChannel(int) { return true; }

bool SyrberusVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SyrberusSound*> (sound) != nullptr;
}

void SyrberusVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    osc.setFrequency(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
    //syrOsc.setFrequency(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
    syrOsc.setKey(midiNoteNumber);
    adsr.noteOn();
    envelope.noteOn();
}

void SyrberusVoice::stopNote(float, bool allowTailOff)
{
    adsr.noteOff();
    envelope.noteOff();

    if (!allowTailOff) {
        clearCurrentNote();
    }
}

void SyrberusVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    voiceBuffer.setSize(outputChannels, samplesPerBlock, false, false, true);
    adsr.setSampleRate(sampleRate);
    envelope.setSampleRate(sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = outputChannels;
    osc.prepare(spec);
    syrOsc.prepare(spec);

    gain.prepare(spec);
    gain.setRampDurationSeconds(0.005f);

    isPrepared = true;
}


void SyrberusVoice::updateParams(float normalizedGain, dubu::EnvelopeGraph* envelopeGraph)
{
    gain.setGainLinear(normalizedGain);
    envelope.setGraph(envelopeGraph);
    
    // We gonna get rid of this one
    adsrParams.attack = envelopeGraph->attack;
    adsrParams.decay = envelopeGraph->decay;
    adsrParams.sustain = envelopeGraph->sustain;
    adsrParams.release = envelopeGraph->release;
    adsr.setParameters(adsrParams);
}

void SyrberusVoice::setUnison(int voices, float detune) {
    syrOsc.setUnison(voices, detune);
}

void SyrberusVoice::updateParams(UnisonVoice::SyrberusOscillatorParams params)
{
    syrOsc.updateParams(params);
}

void SyrberusVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    jassert(isPrepared);

    if (!isVoiceActive()) {
        return;
    }
    

    //juce::dsp::AudioBlock<float> audioBlock{ outputBuffer };
    //osc.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    //gain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    //adsr.applyEnvelopeToBuffer(outputBuffer, startSample, numSamples);


    /*if (voiceBuffer.getNumSamples() != numSamples) {
        voiceBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
    }
    jassert(voiceBuffer.getNumSamples() == numSamples);*/
    
    voiceBuffer.clear(0, numSamples);
    juce::dsp::AudioBlock<float> audioBlock { voiceBuffer };

    // So every other function accepts `numSamples` as an argument. Great!
    // But turns out the oscillator `process` function taking in an audioBlock, does not.
    // Instead I had to use `audioBlock.getSubBlock` to limit it to `numSamples`.
    // Took me 12 hours to figure out this was my issue all along. All I was having
    // is random crackling noises.
    //osc.process(juce::dsp::ProcessContextReplacing<float>(audioBlock.getSubBlock(0, numSamples))); // correct numSamples?
    syrOsc.process(voiceBuffer, 0, numSamples);
    gain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));


    //adsr.applyEnvelopeToBuffer(voiceBuffer, 0, numSamples);
    envelope.applyToBuffer(voiceBuffer, 0, numSamples);

    for (int channel = 0; channel < outputBuffer.getNumChannels(); channel++)
    {
        outputBuffer.addFrom(channel, startSample, voiceBuffer, channel, 0, numSamples);
    }

    if (!envelope.isActive()) {
        clearCurrentNote();
    }
}

void SyrberusVoice::pitchWheelMoved(int newPitchWheelValue)
{

}

void SyrberusVoice::controllerMoved(int controllerNumber, int newControllerValue)
{

}