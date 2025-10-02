/*
  ==============================================================================

    WavePreview.h
    Created: 11 Feb 2025 5:14:07pm
    Author:  Norb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class WavePreview : public juce::AnimatedAppComponent
{

public:
    void update() override;
    void paint(juce::Graphics& g) override;
    void attachApvts(juce::AudioProcessorValueTreeState* apvts);

private:
    juce::AudioProcessorValueTreeState* apvts;

};