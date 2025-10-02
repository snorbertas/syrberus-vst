/*
  ==============================================================================

    SliderLookAndFeel.cpp
    Created: 9 Feb 2025 4:41:27pm
    Author:  Norb

  ==============================================================================
*/

#include "MainLookAndFeel.h"

void MainLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
    float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    // Custom drawing code here (e.g., change shape, colors, shadows)
    auto radius = (float)juce::jmin(width / 2, width / 2) - 4.0f;
    auto centreX = (float)x + (float)width * 0.5f;
    auto centreY = (float)y + (float)width * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // fill
    g.setColour(juce::Colour::fromRGB(32, 28, 31));
    g.fillEllipse(rx, ry, rw, rw);

    // fill progress bg
    g.setColour(juce::Colour::fromRGB(64, 57, 62));
    juce::Path progBg;
    progBg.addArc(rx + 4, ry + 4, rw - 8, rw - 8, rotaryStartAngle, rotaryEndAngle, true);
    g.strokePath(progBg, juce::PathStrokeType(4.0f));

    // fill actual progress
    g.setColour(juce::Colour::fromRGB(253, 97, 254));
    juce::Path prog;
    prog.addArc(rx + 4, ry + 4, rw - 8, rw - 8, rotaryStartAngle, angle, true);
    g.strokePath(prog, juce::PathStrokeType(4.0f));

    juce::Path p;
    auto pointerLength = radius * 0.43f;
    auto pointerThickness = 2.0f;
    p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

    // pointer
    g.setColour(juce::Colours::white);
    g.fillPath(p);

    // text
    g.drawSingleLineText(slider.getName(), centreX, y + height - 4, juce::Justification::horizontallyCentred);

    // any texts in the center?
    if (slider.getProperties().contains("displayInt")) {
        int i = static_cast<int>(slider.getValue());
        g.drawSingleLineText(std::to_string(i), centreX, centreY + 5, juce::Justification::horizontallyCentred);
    }
    if (slider.getProperties().contains("isDetune")) {
        int i = static_cast<int>(slider.getValue() * 100);
        g.drawSingleLineText(std::to_string(i) + "%", centreX, centreY + 5, juce::Justification::horizontallyCentred);
    }
}

void MainLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    bool isOn = button.getToggleState();

    float size = bounds.getHeight();

    // Background
    g.setColour(juce::Colour::fromRGB(32, 28, 31));
    g.fillRoundedRectangle(0.0f, 0.0f, size, size, 5.0f);

    // Draw tick mark if toggled ON
    if (isOn)
    {
        g.setColour(juce::Colour::fromRGB(253, 97, 254));
        juce::Path tick;
        tick.startNewSubPath(size * 0.2f, size * 0.5f);
        tick.lineTo(size * 0.45f, size * 0.7f);
        tick.lineTo(size * 0.7f, size * 0.25f);
        g.strokePath(tick, juce::PathStrokeType(3.0f));
    }

    g.setColour(juce::Colours::white);
    g.drawSingleLineText(button.getButtonText(), size + 6, size - 5, juce::Justification::left);
}