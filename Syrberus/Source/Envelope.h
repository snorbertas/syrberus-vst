/*
  ==============================================================================

    Envelope.h
    Created: 14 Feb 2025 1:16:32pm
    Author:  Norb

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace dubu {

    // UI? 
    class EnvelopeGraph {
    public:
        float attack, decay, sustain, release, timeDecayEnds, timeReleaseEnds;

        float getMaxLengthInSeconds() { return maxLength; };

        void setParams(float attack, float decay, float sustain, float release) {
            this->attack = attack;
            this->decay = decay;
            this->sustain = sustain;
            this->release = release;
            this->timeDecayEnds = attack + decay;
            this->timeReleaseEnds = timeDecayEnds + release;

            if(attack < 0.001f)

            maxLength = attack + sustain + release;
            if (maxLength < 5.0f) maxLength = 5.0f;
        }

        float getValueAtSeconds(float seconds) {
            //
        }

        float getAttack(float seconds) {
            float t = seconds / attack;
            return juce::jmap<float>(t, 0.0f, 1.0f);
        }

        float getDecay(float seconds) {
            float t = (seconds - attack) / decay;
            return juce::jmap<float>(t, 1.0f, sustain);
        }

        float getNormalizedRelease(float seconds) {
            float t = (seconds - attack - decay) / release;
            return juce::jmap<float>(t, 1.0f, 0.0f);
        }

        float getValueAtNormalized(float x) {
            return 0.0f;
        }
    private:
        float maxLength;
    };



    class Envelope {
    public:
        
        void setGraph(EnvelopeGraph* envelopeGraph) {
            env = envelopeGraph;
        }

        void setSampleRate(int sampleRate) {
            this->sampleRate = sampleRate;
            this->sampleDuration = 1.0f / sampleRate;
        }

        void applyToBuffer(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) {

            // sample rate = sampels per second
            // so we need to convert our numSamples to duration in seconds
            float duration = sampleDuration * numSamples;

            // so let's define the time range
            currentTime += duration;

            // and now we just need to grab the values

            float targetGain = updateStateAndCalculateGain();

            // Clear the buffer if it's appropiate (it's cheaper than ramping!)
            if (lastGain == 0.0f && targetGain == 0.0f) {
                buffer.clear();
            } else {
                buffer.applyGainRamp(startSample, numSamples, lastGain, targetGain);
            }

            lastGain = targetGain;
        }

        float updateStateAndCalculateGain() {
            // mute if inactive
            if (state == INACTIVE)
                return 0.0f;

            // still sustaining
            if (state == SUSTAINING)
                return env->sustain;

            // releasing
            if (state == RELEASING) {
                if (currentTime > env->timeReleaseEnds) {
                    state = INACTIVE;
                    return 0.0f;
                }
                return releaseBeginGain * env->getNormalizedRelease(currentTime);
            }

            // Attacking?
            if (currentTime < env->attack) {
                state = ATTACKING;
                return env->getAttack(currentTime);
            }

            // Decaying?
            if (currentTime < env->timeDecayEnds) {
                state = DECAYING;
                return env->getDecay(currentTime);
            }

            // if we reached so far, means we are sustaining
            // currentTime is irrelevant at this point, we just return
            // the sustain gain
            state = SUSTAINING;
            return env->sustain;
        }

        bool isActive() {
            bool isSilentRelease = (state == RELEASING && env->sustain == 0.0f);
            bool isInactive = (state == INACTIVE);
            return !isSilentRelease && !isInactive;
        }

        void noteOn() {
            currentTime = 0;
            state = ATTACKING;
        }

        void noteOff() {
            if (!env) {
                state = INACTIVE;
                return;
            }
            currentTime = env->attack + env->decay; //fast forward to the release
            releaseBeginGain = lastGain;
            state = RELEASING;
        }

    private:
        EnvelopeGraph* env;
        float currentTime = 0;
        int sampleRate;
        float lastGain = 0.0f;
        float releaseBeginGain; // this is what we have to multiply with the release value
        float sampleDuration;
        enum EnvelopeState {
            INACTIVE, ATTACKING, DECAYING, SUSTAINING, RELEASING
        } state = INACTIVE;
        
    };
}