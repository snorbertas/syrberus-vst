/*
  ==============================================================================

    Parameters.h
    Created: 1 Oct 2025 8:00:59pm
    Author:  Norb

  ==============================================================================
*/

#pragma once

namespace Params {
    // Envelope
    inline constexpr auto envAttack = "ENV_ATTACK";
    inline constexpr auto envHold = "ENV_HOLD";
    inline constexpr auto envDecay = "ENV_DECAY";
    inline constexpr auto envSustain = "ENV_SUSTAIN";
    inline constexpr auto envRelease = "ENV_RELEASE";

    // Unison
    inline constexpr auto unisonVoices = "UNISON_VOICES";
    inline constexpr auto unisonDetune = "UNISON_DETUNE";

    // Misc
    inline constexpr auto miscGain = "MISC_GAIN";

    // Osc 1
    inline constexpr auto osc1Shape = "OSC1_SHAPE";
    inline constexpr auto osc1Phase = "OSC1_PHASE";
    inline constexpr auto osc1Invert = "OSC1_INVERT";
    inline constexpr auto osc1Transpose = "OSC1_TRANSPOSE";
    inline constexpr auto osc1Stereo = "OSC1_STEREO";
    inline constexpr auto osc1Mix = "OSC1_MIX";

    // Osc 2
    inline constexpr auto osc2Shape = "OSC2_SHAPE";
    inline constexpr auto osc2Phase = "OSC2_PHASE";
    inline constexpr auto osc2Invert = "OSC2_INVERT";
    inline constexpr auto osc2Transpose = "OSC2_TRANSPOSE";
    inline constexpr auto osc2Stereo = "OSC2_STEREO";
    inline constexpr auto osc2Mix = "OSC2_MIX";

    // Osc 3
    inline constexpr auto osc3Shape = "OSC3_SHAPE";
    inline constexpr auto osc3Phase = "OSC3_PHASE";
    inline constexpr auto osc3Invert = "OSC3_INVERT";
    inline constexpr auto osc3Transpose = "OSC3_TRANSPOSE";
    inline constexpr auto osc3Stereo = "OSC3_STEREO";
    inline constexpr auto osc3Mix = "OSC3_MIX";

    // Osc X
    inline auto oscShape(int oscId) {
        return std::string("OSC") + std::to_string(oscId) + "_SHAPE";
    }
    inline auto oscPhase(int oscId) {
        return std::string("OSC") + std::to_string(oscId) + "_PHASE";
    }
    inline auto oscInvert(int oscId) {
        return std::string("OSC") + std::to_string(oscId) + "_INVERT";
    }
    inline auto oscTranspose(int oscId) {
        return std::string("OSC") + std::to_string(oscId) + "_TRANSPOSE";
    }
    inline auto oscStereo(int oscId) {
        return std::string("OSC") + std::to_string(oscId) + "_STEREO";
    }
    inline auto oscMix(int oscId) {
        return std::string("OSC") + std::to_string(oscId) + "_MIX";
    }
}

