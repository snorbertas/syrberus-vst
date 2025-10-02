/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MainLookAndFeel.h"
#include "ShapeSelectButton.h"
#include "WavePreview.h"
#include "DebugInfo.h"
#include "EnvelopeEditor.h"

//==============================================================================
/**
*/
class SyrberusAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SyrberusAudioProcessorEditor (SyrberusAudioProcessor&);
    ~SyrberusAudioProcessorEditor() override;

    struct OscilatorGroup {
        ShapeSelectButton sine, square, triangle, saw, sineSquare;
        juce::Slider phase;
        juce::ToggleButton invert;
        juce::Slider transpose;
        juce::Slider stereo;
        juce::Slider mix;

        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> phaseAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> invertAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> transposeAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> stereoAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    };

    enum RadioGroups {
        Oscilator1 = 100,
        Oscilator2 = 101,
        Oscilator3 = 102,
    };

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void initKnob(juce::Slider& slider, std::string key, std::string name, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> &attachment);
    void initOsc(OscilatorGroup& osc, int oscId, RadioGroups group);
    void initShapeButton(ShapeSelectButton& button, std::string paramId, RadioGroups group, int shapeId);
    void positionOsc(OscilatorGroup& osc, int offsetY);
    void positionEnvelope();
    void paintWaveform(juce::Graphics& g);
    void paintEnvelope(juce::Graphics& g);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SyrberusAudioProcessor& audioProcessor;

    dubu::EnvelopeEditor envelopeEditor;
    WavePreview wavePreview;
    DebugInfo debugInfo;

    MainLookAndFeel sliderLaf;

    juce::MidiKeyboardComponent keyboardComponent;
    juce::Slider envAttack;
    juce::Slider envHold;
    juce::Slider envDecay;
    juce::Slider envSustain;
    juce::Slider envRelease;
    juce::Slider unisonVoices;
    juce::Slider unisonDetune;
    juce::Slider miscGain;
    OscilatorGroup osc1, osc2, osc3;

    juce::Image imgLogo;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> envAttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> envHoldAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> envDecayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> envSustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> envReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> unisonVoicesAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> unisonDetuneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> miscGainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SyrberusAudioProcessorEditor)

};
