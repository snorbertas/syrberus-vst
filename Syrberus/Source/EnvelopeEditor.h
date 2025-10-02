/*
  ==============================================================================

    EnvelopeEditor.h
    Created: 14 Feb 2025 4:01:26pm
    Author:  Norb

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Envelope.h"

namespace dubu {
    class EnvelopeEditor : public juce::AnimatedAppComponent {
    public:
        void update() override
        {

        }
        void paint(juce::Graphics& g) override
        {
            auto w = getBounds().getWidth();
            auto h = getBounds().getHeight();
            auto maxSeconds = env->getMaxLengthInSeconds();
            auto attack = (env->attack / maxSeconds) * w;
            auto decay = (env->decay / maxSeconds) * w;
            auto sustain = (1.0f - env->sustain) * h;
            auto release = (env->release / maxSeconds) * w;

            g.setColour(juce::Colours::white);


            juce::Path p;
            p.startNewSubPath(0.0f, h);
            p.lineTo(attack, 0.0f);
            p.lineTo(attack + decay, sustain);
            p.lineTo(attack + decay + release, h);

            g.strokePath(p, juce::PathStrokeType(2.0f));
        }

        void attachApvts(juce::AudioProcessorValueTreeState* apvts)
        {
            this->apvts = apvts;
        }

        void attachEnvelopeGraph(dubu::EnvelopeGraph* envelopeGraph) {
            env = envelopeGraph;
        }

    private:
        juce::AudioProcessorValueTreeState* apvts;
        dubu::EnvelopeGraph* env;

    };
}