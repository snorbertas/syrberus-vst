/*
  ==============================================================================

    ShapeSelectButton.cpp
    Created: 11 Feb 2025 3:22:49pm
    Author:  Norb

  ==============================================================================
*/

#include "ShapeSelectButton.h"

void ShapeSelectButton::initShapeSelect(juce::AudioProcessorValueTreeState& treeState, const juce::String& paramId, int shapeId)
{
    this->shapeId = shapeId;
    this->apvts = &treeState;
    parameterId = paramId;
    treeState.addParameterListener(parameterId, this);

    // force the initial state update
    bool toggleOn = static_cast<int>(std::round(apvts->getParameter(parameterId)->getValue() * 4)) == shapeId;
    setToggleState(toggleOn, false);

    onStateChange = [&]()
        {
            toggleChanged();
        };
}

void ShapeSelectButton::parameterChanged(const juce::String& parameterId, float newValue)
{
    // is the parameter value my shape id?
    bool toggleOn = static_cast<int>(std::round(newValue)) == shapeId;

    juce::MessageManager::callAsync([&]() {
            setToggleState(toggleOn, false);
            repaint();
        });
}

void ShapeSelectButton::toggleChanged()
{
    // change the parameter
    if (!apvts) return;
    if(getToggleState())
        apvts->getParameter(parameterId)->setValue(shapeId / 4.0f);
}

void ShapeSelectButton::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();
    bool isOn = getToggleState();
    float size = bounds.getHeight();

    // Background
    auto colorOff = juce::Colour::fromRGB(32, 28, 31);
    auto colorOn = juce::Colour::fromRGB(253, 97, 254);
    g.setColour(isOn ? colorOn : colorOff);
    g.fillRoundedRectangle(0.0f, 0.0f, size, size, 5.0f);


    // Draw shape
    g.setColour(isOn ? juce::Colours::white : juce::Colours::darkgrey);
    juce::Path p;
    float amplitude = size / 4;
    float frequency = 2.0f * juce::MathConstants<float>::pi / (size - 6);
    p.startNewSubPath(3, size / 2);


    if (shapeId == 0) {
        for (float x = 0; x < size - 6; x++)
        {
            float y = size / 2 + amplitude * std::sin(frequency * x - juce::MathConstants<float>::pi);
            p.lineTo(x + 3, y);
        }
        g.strokePath(p, juce::PathStrokeType(2.0f));
        return;
    }

    if (shapeId == 1) {
        p.lineTo(3, size / 2 - amplitude);
        p.lineTo(3 + (size - 6) * 0.5f, size / 2 - amplitude);
        p.lineTo(3 + (size - 6) * 0.5f, size / 2 + amplitude);
        p.lineTo(3 + size - 6, size / 2 + amplitude);
        p.lineTo(3 + size - 6, size / 2);
        g.strokePath(p, juce::PathStrokeType(2.0f));
        return;
    }

    if (shapeId == 2) {
        p.lineTo(3 + (size - 6) * 0.25f, size / 2 - amplitude);
        p.lineTo(3 + (size - 6) * 0.75f, size / 2 + amplitude);
        p.lineTo(3 + size - 6, size / 2);
        g.strokePath(p, juce::PathStrokeType(2.0f));
        return;
    }

    if (shapeId == 3) {
        p.lineTo(3 + (size - 6) * 0.5f, size / 2 - amplitude);
        p.lineTo(3 + (size - 6) * 0.5f, size / 2 + amplitude);
        p.lineTo(3 + size - 6, size / 2);
        g.strokePath(p, juce::PathStrokeType(2.0f));
        return;
    }

    if (shapeId == 4) {
        for (float x = 0; x < (size - 6) * 0.5f; x++)
        {
            float y = size / 2 + amplitude * std::sin(frequency * x - juce::MathConstants<float>::pi);
            p.lineTo(x + 3, y);
        }
        p.lineTo(3 + (size - 6) * 0.5f, size / 2 + amplitude);
        p.lineTo(3 + size - 6, size / 2 + amplitude);
        p.lineTo(3 + size - 6, size / 2);
        g.strokePath(p, juce::PathStrokeType(2.0f));
        return;
    }
}