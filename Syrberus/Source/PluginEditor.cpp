/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters.h"


void SyrberusAudioProcessorEditor::initKnob(juce::Slider &slider, std::string key, std::string name,
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> &attachment)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    slider.setLookAndFeel(&sliderLaf);
    slider.setName(name);

    addAndMakeVisible(slider);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, key, slider);
}

void SyrberusAudioProcessorEditor::initShapeButton(ShapeSelectButton& button, std::string paramId, RadioGroups group, int shapeId)
{
    button.setRadioGroupId(group);
    button.initShapeSelect(audioProcessor.apvts, paramId, shapeId);
    addAndMakeVisible(button);
}

void SyrberusAudioProcessorEditor::initOsc(OscilatorGroup& osc, int oscId, RadioGroups group)
{
    initKnob(osc.phase, Params::oscPhase(oscId), "Phase", osc.phaseAttachment);
    initKnob(osc.transpose, Params::oscTranspose(oscId), "Transpose", osc.transposeAttachment);
    osc.transpose.getProperties().set("displayInt", true);
    initKnob(osc.stereo, Params::oscStereo(oscId), "Stereo", osc.stereoAttachment);
    initKnob(osc.mix, Params::oscMix(oscId), "Mix", osc.mixAttachment);

    auto shapeParamId = Params::oscShape(oscId);
    initShapeButton(osc.sine, shapeParamId, group, 0);
    initShapeButton(osc.square, shapeParamId, group, 1);
    initShapeButton(osc.triangle, shapeParamId, group, 2);
    initShapeButton(osc.saw, shapeParamId, group, 3);
    initShapeButton(osc.sineSquare, shapeParamId, group, 4);

    osc.invert.setButtonText("Invert");
    osc.invert.setLookAndFeel(&sliderLaf);
    osc.invertAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, Params::oscInvert(oscId), osc.invert);

    addAndMakeVisible(osc.invert);
}

//==============================================================================
SyrberusAudioProcessorEditor::SyrberusAudioProcessorEditor (SyrberusAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    keyboardComponent(p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    addAndMakeVisible(keyboardComponent);

    initKnob(envAttack, Params::envAttack, "Attack", envAttackAttachment);
    initKnob(envHold, Params::envHold, "Hold", envHoldAttachment);
    initKnob(envDecay, Params::envDecay, "Decay", envDecayAttachment);
    initKnob(envSustain, Params::envSustain, "Sustain", envSustainAttachment);
    initKnob(envRelease, Params::envRelease, "Release", envReleaseAttachment);

    initKnob(unisonVoices, Params::unisonVoices, "Voices", unisonVoicesAttachment);
    initKnob(unisonDetune, Params::unisonDetune, "Detune", unisonDetuneAttachment);
    unisonVoices.getProperties().set("displayInt", true);
    unisonDetune.getProperties().set("isDetune", true);

    initKnob(miscGain, Params::miscGain, "Gain", miscGainAttachment);

    initOsc(osc1, 1, RadioGroups::Oscilator1);
    initOsc(osc2, 2, RadioGroups::Oscilator2);
    initOsc(osc3, 3, RadioGroups::Oscilator3);

    wavePreview.attachApvts(&audioProcessor.apvts);
    wavePreview.setFramesPerSecond(60);
    wavePreview.setOpaque(false);
    addAndMakeVisible(wavePreview);

    debugInfo.init(&p.synth);
    debugInfo.setFramesPerSecond(60);
    debugInfo.setOpaque(false);
    addAndMakeVisible(debugInfo);

    envelopeEditor.attachApvts(&audioProcessor.apvts);
    envelopeEditor.attachEnvelopeGraph(&p.envelopeGraph);
    envelopeEditor.setFramesPerSecond(60);
    envelopeEditor.setOpaque(false);
    addAndMakeVisible(envelopeEditor);

    // load images
    imgLogo = juce::ImageFileFormat::loadFrom(BinaryData::logo_png, BinaryData::logo_pngSize);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(918, 600);
}

SyrberusAudioProcessorEditor::~SyrberusAudioProcessorEditor() { }

void SyrberusAudioProcessorEditor::paintWaveform(juce::Graphics& g)
{
    /*int startX = 15;
    int startY = 395;
    int width = 500;
    int height = 130;
    g.setColour(juce::Colour::fromRGB(32, 28, 31));
    g.fillRoundedRectangle(startX, startY, width, height, 4.0f);*/
}

void SyrberusAudioProcessorEditor::paintEnvelope(juce::Graphics& g)
{
    int startX = 548;
    int startY = 38;
    int width = 340;
    int height = 100;
    g.setColour(juce::Colour::fromRGB(32, 28, 31));
    g.fillRoundedRectangle(startX, startY, width, height, 4.0f);
}

//==============================================================================
void SyrberusAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll(juce::Colour::fromRGB(68, 20, 19));
    g.setGradientFill(juce::ColourGradient::vertical(
        juce::Colour::fromRGB(61, 41, 104), 0.0f,
        juce::Colour::fromRGB(35, 27, 65), 600.0f
    ));
    g.fillAll();


    juce::Colour gradientTop = juce::Colour::fromRGB(103, 64, 168);
    //juce::Colour gradientBottom = juce::Colour::fromRGB(72, 28, 18);
    juce::Colour gradientBottom = juce::Colour::fromRGB(88, 49, 153);
    float roundingAngle = 4.0f;

    //g.setColour(juce::Colour::fromRGBA(191, 76, 45, 222));
    g.setGradientFill(juce::ColourGradient::vertical(
        gradientTop, 0.0f,
        gradientBottom, 200.0f
    ));
    g.fillRoundedRectangle(10, 10, 508, 110, roundingAngle); //osc1

    g.setGradientFill(juce::ColourGradient::vertical(
        gradientTop, 0.0f,
        gradientBottom, 400.0f
    ));
    g.fillRoundedRectangle(528, 10, 380, 230, roundingAngle); //envelope

    g.setGradientFill(juce::ColourGradient::vertical(
        gradientTop, 130.0f,
        gradientBottom, 330.0f
    ));
    g.fillRoundedRectangle(10, 130, 508, 110, roundingAngle); //osc2

    g.setGradientFill(juce::ColourGradient::vertical(
        gradientTop, 250.0f,
        gradientBottom, 450.0f
    ));
    g.fillRoundedRectangle(10, 250, 508, 110, roundingAngle); //osc3
    g.fillRoundedRectangle(528, 250, 250, 120, roundingAngle); //unison
    g.fillRoundedRectangle(528 + 250 + 10, 250, 120, 120, roundingAngle); //misc

    g.setGradientFill(juce::ColourGradient::vertical(
        gradientTop, 370.0f,
        gradientBottom, 570.0f
    ));
    //g.fillRoundedRectangle(10, 370, 508, 160, roundingAngle); //waveform preview


    //g.setColour(juce::Colour::fromRGB(16, 14, 16));
    //g.fillRect(20, 60, 200, 86);

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));

    g.drawSingleLineText("OSC 1", 20, 27, juce::Justification::left);
    g.drawSingleLineText("OSC 2", 20, 147, juce::Justification::left);
    g.drawSingleLineText("OSC 3", 20, 267, juce::Justification::left);
    g.drawSingleLineText("Unison", 538, 267, juce::Justification::left);
    g.drawSingleLineText("Misc", 798, 267, juce::Justification::left);
    g.drawSingleLineText("Envelope", 538, 27, juce::Justification::left);
    g.drawSingleLineText("Waveform Preview (x4)", 20, 387, juce::Justification::left);

    //paintWaveform(g);
    paintEnvelope(g);
    g.drawImageAt(imgLogo, 572, 400);
}

void SyrberusAudioProcessorEditor::positionOsc(OscilatorGroup& osc, int offsetY)
{
    float knobSize = 64;
    float gap = knobSize + 4;
    float knobHeight = knobSize + 16;
    float oscKnobStartX = 237;
    float oscKnobStartY = 25 + offsetY;
    osc.transpose.setBounds(oscKnobStartX + gap * 0, oscKnobStartY, knobSize, knobHeight);
    osc.phase.setBounds(oscKnobStartX + gap * 1, oscKnobStartY, knobSize, knobHeight);
    osc.stereo.setBounds(oscKnobStartX + gap * 2, oscKnobStartY, knobSize, knobHeight);
    osc.mix.setBounds(oscKnobStartX + gap * 3, oscKnobStartY, knobSize, knobHeight);

    float shapeStartX = 20;
    float shapePosY = 40 + offsetY;
    float shapeSize = 38;
    float shapeGap = shapeSize + 4;
    osc.sine.setBounds(shapeStartX + shapeGap * 0, shapePosY, shapeSize, shapeSize);
    osc.square.setBounds(shapeStartX + shapeGap * 1, shapePosY, shapeSize, shapeSize);
    osc.triangle.setBounds(shapeStartX + shapeGap * 2, shapePosY, shapeSize, shapeSize);
    osc.saw.setBounds(shapeStartX + shapeGap * 3, shapePosY, shapeSize, shapeSize);
    osc.sineSquare.setBounds(shapeStartX + shapeGap * 4, shapePosY, shapeSize, shapeSize);
    osc.invert.setBounds(20, 90 + offsetY, 210, 20);
}

void SyrberusAudioProcessorEditor::positionEnvelope()
{
    float knobSize = 64;
    float gap = knobSize + 4;
    float knobHeight = knobSize + 16;
    float oscKnobStartX = 550;
    float oscKnobStartY = 148;
    envAttack.setBounds(oscKnobStartX + gap * 0, oscKnobStartY, knobSize, knobHeight);
    //envHold.setBounds(oscKnobStartX + gap * 1, oscKnobStartY, knobSize, knobHeight);
    envDecay.setBounds(oscKnobStartX + gap * 2, oscKnobStartY, knobSize, knobHeight);
    envSustain.setBounds(oscKnobStartX + gap * 3, oscKnobStartY, knobSize, knobHeight);
    envRelease.setBounds(oscKnobStartX + gap * 4, oscKnobStartY, knobSize, knobHeight);
}

void SyrberusAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    keyboardComponent.setBounds(0, getHeight() - 60, getWidth(), 60);

    positionEnvelope();

    float y = 276;
    unisonVoices.setBounds(550, y, 64, 64 + 16);
    unisonDetune.setBounds(550 + 68, y, 64, 64 + 16);
    miscGain.setBounds(815, y, 64, 64 + 16);

    positionOsc(osc1, 0);
    positionOsc(osc2, 120);
    positionOsc(osc3, 240);

    wavePreview.setBounds(15, 395, 500, 130);
    debugInfo.setBounds(528, 395 + 90, 100, 100);
    envelopeEditor.setBounds(548, 38, 340, 100);
}
