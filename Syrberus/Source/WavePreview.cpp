/*
  ==============================================================================

    WavePreview.cpp
    Created: 11 Feb 2025 5:14:07pm
    Author:  Norb

  ==============================================================================
*/

#include "WavePreview.h"
#include <cmath>  // For the pow function
#include "Parameters.h"

void WavePreview::update() {
    repaint();
}

float getSineWave(float x, float freq, float lengthUnit) {
    // x 0 ~ 4
    float frequency = 2.0f * juce::MathConstants<float>::pi / lengthUnit * freq;
    return std::sin(frequency * x);
}

float getSquareWave(float x, float freq, float lengthUnit) {
    float frequency = 2.0f * juce::MathConstants<float>::pi / lengthUnit * freq;
    return std::sin(frequency * x) >= 0 ? 1.0f : -1.0f;
}

float getTriangleWave(float x, float freq, float lengthUnit) {
    float frequency = lengthUnit / freq;
    float quarter = frequency * 0.25f;
    float t = fmod(x, frequency);
    if (t < quarter) return t / quarter;
    if (t < quarter*3) return 1.0f - ((t - quarter) / (quarter * 2)) * 2.0f;
    return (t - quarter*3) / quarter - 1.0f;
}

float getSawWave(float x, float freq, float lengthUnit) {
    float frequency = lengthUnit / freq;
    float half = frequency * 0.5f;
    float t = fmod(x, frequency);
    if (t < half) return (t / half);
    return ((t - half) / half) - 1.0f;
}

float getSineSquareWave(float x, float freq, float lengthUnit) {
    float frequency = lengthUnit / freq;
    float half = frequency * 0.5f;
    float t = fmod(x, frequency);
    if (t < half) return getSineWave(x, freq, lengthUnit);
    return getSquareWave(x, freq, lengthUnit);
}

float getWave(int shapeId, bool invert, float x, float freq, float lengthUnit) {
    float result = 0.0f;
    if (shapeId == 0) result = getSineWave(x, freq, lengthUnit);
    if (shapeId == 1) result = getSquareWave(x, freq, lengthUnit);
    if (shapeId == 2) result = getTriangleWave(x, freq, lengthUnit);
    if (shapeId == 3) result = getSawWave(x, freq, lengthUnit);
    if (shapeId == 4) result = getSineSquareWave(x, freq, lengthUnit);
    return result * (invert ? -1.0f : 1.0f);
}

void WavePreview::paint(juce::Graphics& g)
{
    auto width = getBounds().getWidth();
    auto oneLengthUnit = width / 4.0f;
    auto height = getBounds().getHeight();
    auto centerY = height * 0.5f;

    g.setColour(juce::Colour::fromRGB(32, 28, 31));
    g.fillRoundedRectangle(0, 0, width, height, 4.0f);

    g.setColour(juce::Colours::white.withAlpha(0.02f));
    g.drawHorizontalLine(centerY + height / 3, 0, width);
    g.drawHorizontalLine(centerY - height / 3, 0, width);
    g.setColour(juce::Colours::white.withAlpha(0.05f));
    g.drawHorizontalLine(centerY, 0, width);
    for (int i = 1; i < 4; i++) {
        g.drawVerticalLine(oneLengthUnit * i, 0, height);
    }

    // extract all the values and try visualize it, we'll refactor this into its own class later and utilize actual Oscilators
    float shape[3], weight[3], transpose[3], phase[3]; bool invert[3];
    for (int i = 0; i < 3; i++) {
        int id = i + 1;
        shape[i] = static_cast<int>(std::round(apvts->getParameter(Params::oscShape(id))->getValue() * 4));
        weight[i] = apvts->getParameter(Params::oscMix(id))->getValue();
        transpose[i] = static_cast<int>(std::round(apvts->getParameter(Params::oscTranspose(id))->getValue() * 48)) - 24;
        phase[i] = apvts->getParameter(Params::oscPhase(id))->getValue();
        invert[i] = apvts->getParameter(Params::oscInvert(id))->getValue() > 0.5f;
    }
    float totalWeight = weight[0] + weight[1] + weight[2];
    if (totalWeight < 1.0f) totalWeight = 1.0f; // min total weight of 1 to allow shaping of single osc
    float miscGain = apvts->getParameter(Params::miscGain)->getValue();
    
    // In reality the oscilators will create the shape, but I want to
    // develop a deeper understanding how waves behave mathematically.
    // The shape produced in this preview will be compared with the
    // actual shapes produced by the oscilators inside a DAW.

    // Firstly, we have to establish that an octave (12 semitones)
    // doubles/halves the wavelength. We split the preview into 4
    // horizontal slices. That's because we can go down to -24 semitones
    // on any oscillator, which is 4 times longer than the base 0.

    // frequency ratio equation:
    // 2^(transpose/12)
    float freq1 = powf(2.0f, transpose[0] / 12.0f);
    float freq2 = powf(2.0f, transpose[1] / 12.0f);
    float freq3 = powf(2.0f, transpose[2] / 12.0f);

    juce::Path sinePath;
    float amplitude = (height / 3) * (miscGain * 2.0f);

    sinePath.startNewSubPath(0, height / 2); // Start in the middle
    for (float x = 0; x < width; x++)
    {
        float px1 = x + oneLengthUnit * phase[0];
        float px2 = x + oneLengthUnit * phase[1];
        float px3 = x + oneLengthUnit * phase[2];
        float wave1 = getWave(shape[0], invert[0], px1, freq1, oneLengthUnit) * weight[0];
        float wave2 = getWave(shape[1], invert[1], px2, freq2, oneLengthUnit) * weight[1];
        float wave3 = getWave(shape[2], invert[2], px3, freq3, oneLengthUnit) * weight[2];

        float y = centerY - amplitude * ((wave1 + wave2 + wave3) / totalWeight);
        //float y = height / 2 - amplitude * getSineSquareWave(x, freq1, oneLengthUnit);
        sinePath.lineTo(x, y);
    }

    g.setColour(juce::Colour::fromRGB(253, 97, 254));
    g.strokePath(sinePath, juce::PathStrokeType(2.0f)); // Draw the path
}

void WavePreview::attachApvts(juce::AudioProcessorValueTreeState* apvts)
{
    this->apvts = apvts;
}