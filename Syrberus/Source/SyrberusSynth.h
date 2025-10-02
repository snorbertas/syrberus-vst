/*
  ==============================================================================

    SyrberusSynth.h
    Created: 11 Feb 2025 11:19:50pm
    Author:  Norb

  ==============================================================================
*/

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <JuceHeader.h>
#include "SyrberusOscillator.h"
#include "SyrberusOscillator.h"
#include "Envelope.h"

// Represents a basic synth sound
class SyrberusSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override;
    bool appliesToChannel(int) override;
};

// Generates a sine wave for each note played
class SyrberusVoice : public juce::SynthesiserVoice
{
public:
    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
    void stopNote(float, bool allowTailOff) override;
    void prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels);
    void setUnison(int voices, float detune);
    void updateParams(float normalizedGain, dubu::EnvelopeGraph* envelopeGraph);
    void updateParams(UnisonVoice::SyrberusOscillatorParams params);
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;

private:
    dubu::Envelope envelope;
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;
    juce::AudioBuffer<float> voiceBuffer;

    juce::dsp::Oscillator<float> osc{ [](float x) { return std::sin(x); } };
    SyrberusOscillator syrOsc;
    juce::dsp::Gain<float> gain;
    bool isPrepared = false;
};

