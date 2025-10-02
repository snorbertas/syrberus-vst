/*
  ==============================================================================

    DebugInfo.h
    Created: 12 Feb 2025 8:05:48pm
    Author:  Norb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class DebugInfo : public juce::AnimatedAppComponent {
public:
    void update() override {};
    void paint(juce::Graphics& g) override;
    void init(juce::Synthesiser* synth) {
        debugSynth = synth;
    }

private:
    juce::Synthesiser* debugSynth;
};