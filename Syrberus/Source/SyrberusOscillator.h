/*
  ==============================================================================

    SyrberusOscillator.h
    Created: 13 Feb 2025 11:15:29am
    Author:  Norb

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Parameters.h"

class UnisonVoice {
public:
    enum
    {
        oscIndex,
        pannerIndex,
        gainIndex
    };

    enum WaveType {
        SINE,
        SQUARE,
        TRIANGLE,
        SAW,
        SINE_SQUARE
    };

    struct OscillatorParams {
        WaveType type;
        int transpose;
        float gain;
        float phase;
        float stereo;
        bool invert;
    };

    struct SyrberusOscillatorParams {
        OscillatorParams osc[3];

        SyrberusOscillatorParams(juce::AudioProcessorValueTreeState& apvts) {
            for (int i = 0; i < 3; i++) {
                int id = i + 1;
                osc[i] = OscillatorParams {
                    (UnisonVoice::WaveType)static_cast<int>(std::round(apvts.getParameter(Params::oscShape(id))->getValue() * 4)),
                    static_cast<int>(std::round(apvts.getParameter(Params::oscTranspose(id))->getValue() * 48)) - 24,
                    apvts.getParameter(Params::oscMix(id))->getValue(),
                    apvts.getParameter(Params::oscPhase(id))->getValue(),
                    apvts.getRawParameterValue(Params::oscStereo(id))->load(),
                    apvts.getParameter(Params::oscInvert(id))->getValue() > 0.5f
                };
            }
        }
    };

    void setWaveType(int chain, WaveType type)
    {
        auto updateChain = [](juce::dsp::ProcessorChain<juce::dsp::Oscillator<float>, juce::dsp::Panner<float>, juce::dsp::Gain<float>>& chain, int type)
            {
                auto& osc = chain.template get<oscIndex>();

                if (type == 0) osc.initialise([](float x) { return std::sin(x + juce::MathConstants<float>::pi); }, 128);
                if (type == 1) osc.initialise([](float x) { return std::sin(x + juce::MathConstants<float>::pi) < 0.0f ? -1.0f : 1.0f; }, 128);
                if (type == 2) osc.initialise([](float x) { return std::sin(x + juce::MathConstants<float>::pi); }, 5);
                if (type == 3) osc.initialise([](float x) { return x < 0 ?
                    (1.0f - x / -juce::MathConstants<float>::pi) :
                    (x / juce::MathConstants<float>::pi - 1.0f); }, 128);

                if (type == 4) osc.initialise([](float x) { return
                    x <= 0 ? std::sin(x + juce::MathConstants<float>::pi) : -1.0f;
                    }, 128);
            };

        waveType[chain] = type;
        updateChain(processorChain[chain], type);
    }

    void setKey(int midiKey) {
        for (int i = 0; i < 3; i++) {
            processorChain[i].template get<oscIndex>().setFrequency(juce::MidiMessage::getMidiNoteInHertz((float)midiKey + (float)transpose[i] + unisonDetune));
            processorChain[i].template get<oscIndex>().phase.advance(unisonPhase * 2 * juce::MathConstants<float>::pi);
        }
    }

    void setLevels(float gains[]) {
        // we will adjust the gains by treating them as weights
        float totalWeight = gains[0] + gains[1] + gains[2];
        if (totalWeight < 1.0f) totalWeight = 1.0f; // min total weight of 1 to allow shaping of single osc

        for (int i = 0; i < 3; i++) {
            processorChain[i].template get<gainIndex>().setGainLinear(gains[i] / totalWeight);
        }
    }

    void updateParams(SyrberusOscillatorParams params)
    {
        for (int i = 0; i < 3; i++) {
            // shapes
            if (params.osc[i].type != waveType[i]) setWaveType(i, params.osc[i].type);

            // phase
            if (params.osc[i].phase != phase[i]) {
                float offset = params.osc[i].phase - phase[i];
                if (offset < 0.0f) offset += 1.0f;
                phase[i] = params.osc[i].phase;
                processorChain[i].template get<oscIndex>().phase.advance(offset * 2 * juce::MathConstants<float>::pi);
            }

            // transpose
            transpose[i] = params.osc[i].transpose;
        }

        // gains
        float gains[3]{
            params.osc[0].gain, params.osc[1].gain, params.osc[2].gain
        };
        setLevels(gains);
    }

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        for (int i = 0; i < 3; i++) {
            buffer[i].setSize(spec.numChannels, spec.maximumBlockSize, false, false, true);
            processorChain[i].prepare(spec);
            processorChain[i].template get<gainIndex>().setRampDurationSeconds(0.005f);
            panner[i] = &(processorChain[i].template get<pannerIndex>());
            panner[i]->setRule(juce::dsp::PannerRule::balanced);
        }
    }


    void process(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) noexcept
    {
        for (int i = 0; i < 3; i++) {
            panner[i]->setPan(unisonPan);
            buffer[i].clear(0, numSamples);
            juce::dsp::AudioBlock<float> audioBlock { buffer[i] };
            processorChain[i].process(juce::dsp::ProcessContextReplacing<float>(audioBlock.getSubBlock(0, numSamples)));
        }

        for (int channel = 0; channel < outputBuffer.getNumChannels(); channel++)
        {
            for (int i = 0; i < 3; i++) {
                outputBuffer.addFrom(channel, startSample, buffer[i], channel, 0, numSamples);
            }
        }
    }

    void reset() noexcept
    {
        for (int i = 0; i < 3; i++) {
            processorChain[i].reset();
        }
    }

    // One buffer for each processor chain
    juce::AudioBuffer<float> buffer[3];

    // One processor chain for each oscillator
    juce::dsp::ProcessorChain<juce::dsp::Oscillator<float>, juce::dsp::Panner<float>, juce::dsp::Gain<float>>
        processorChain[3];

    juce::dsp::Panner<float>* panner[3];

    // One wave type for each oscillator
    WaveType waveType[3];
    float phase[3];
    int transpose[3];

    // unison-specific
    float unisonDetune = 0.0f;
    float unisonPhase = 0.0f;
    float unisonPan = 0.0f;
};


/*
 * Handles all of the oscillator logic for a single voice. It comines 3 oscillators
 * of different types.
*/
class SyrberusOscillator {

public:
    SyrberusOscillator()
    {
        setUnison(1, 1.0f);

        for (int i = 0; i < UNISON_COUNT; i++) {
            for(int o = 0; o < 3; o++) {
                unison[i].setWaveType(o, UnisonVoice::WaveType::SINE);
            }
        }
    }

    void setKey(int midiKey) {
        for (int i = 0; i < UNISON_COUNT; i++) {
            unison[i].setKey(midiKey);
        }
    }

    void setLevel(float gains[]) {
        for (int i = 0; i < UNISON_COUNT; i++) {
            unison[i].setLevels(gains);
        }
    }

    void setUnison(int unisonCount, float detune) {
        CURRENT_VOICES = unisonCount;

        if (CURRENT_VOICES == 1) {
            unison[0].unisonPhase = 0.0f;
            unison[0].unisonDetune = 0.0f;
            unison[0].unisonPan = 0.0f;
            return;
        }

        for (int i = 0; i < CURRENT_VOICES; i++) {
            float t = ((float)i / (float)(CURRENT_VOICES - 1));

            unison[i].unisonPhase = 1.0f - t * (0.5f);
            unison[i].unisonDetune = (-0.5f * detune) + t * detune;
            unison[i].unisonPan = -0.5f + t;
        }


    }

    void process(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) noexcept
    {
        for (int i = 0; i < CURRENT_VOICES; i++) {
            unison[i].process(outputBuffer, startSample, numSamples);
        }

        // PUT A LIMITER INSTEAD
        //outputBuffer.applyGain(1.0f / (float)CURRENT_VOICES); 
    }


    void updateParams(UnisonVoice::SyrberusOscillatorParams params)
    {
        for (int i = 0; i < UNISON_COUNT; i++) {
            unison[i].updateParams(params);
        }
    }

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        for (int i = 0; i < UNISON_COUNT; i++) {
            unison[i].prepare(spec);
        }
    }

    void reset() noexcept
    {
        for (int i = 0; i < UNISON_COUNT; i++) {
            unison[i].reset();
        }
    }

private:
    enum { UNISON_COUNT = 17 };
    UnisonVoice unison[UNISON_COUNT];
    int CURRENT_VOICES = -1;
};