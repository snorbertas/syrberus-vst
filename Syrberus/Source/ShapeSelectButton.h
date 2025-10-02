/*
  ==============================================================================

    ShapeSelectButton.h
    Created: 11 Feb 2025 3:22:49pm
    Author:  Norb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class ShapeSelectButton : public juce::ToggleButton, public juce::AudioProcessorValueTreeState::Listener
{
public:
    ~ShapeSelectButton() override
    {
        if (!apvts) return;
        apvts->removeParameterListener(parameterId, this);
    }

    void initShapeSelect(juce::AudioProcessorValueTreeState& treeState, const juce::String& paramId, int shapeId);
    void parameterChanged(const juce::String& parameterId, float newValue) override;
    void toggleChanged();
    void paint(juce::Graphics& g) override;

private:
    int shapeId;
    juce::AudioProcessorValueTreeState* apvts = nullptr;
    juce::String parameterId;
};