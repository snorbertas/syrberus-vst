/*
  ==============================================================================

    DebugInfo.cpp
    Created: 12 Feb 2025 8:05:48pm
    Author:  Norb

  ==============================================================================
*/

#include "DebugInfo.h"

void DebugInfo::paint(juce::Graphics& g) {
    if (!debugSynth) return;

    int totalVoices = debugSynth->getNumVoices();
    int activeVoices = 0;

    for (int i = 0; i < debugSynth->getNumVoices(); i++) {
        if (debugSynth->getVoice(i)->isVoiceActive())
            activeVoices++;
    }

    std::string debugInfo = "Voices: " + std::to_string(totalVoices);
    debugInfo += "\nActive: " + std::to_string(activeVoices);
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawMultiLineText(debugInfo, 2, 22, 200);
}