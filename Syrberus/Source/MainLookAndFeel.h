/*
  ==============================================================================

    SliderLookAndFeel.h
    Created: 9 Feb 2025 4:41:27pm
    Author:  Norb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class MainLookAndFeel : public juce::LookAndFeel_V4
{
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

};
