#pragma once
#include "daisysp.h"

using namespace daisysp;

class SynthEngine {
public:
    void Init(float sample_rate);
    float Process();
    void OnNoteOn(uint8_t note, uint8_t velocity);
    void OnNoteOff();
    void SetLfoDepth(float depth);
    void SetLfoTarget(int target);

    // New method to update envelope parameters
    void SetEnvelopeParameters(float attack, float decay, float sustain, float release);

private:
    Oscillator osc;
    Oscillator lfo;
    Adsr envelope;
    float frequency;
    float lfo_depth;
    int lfo_target; // 0: Pitch, 1: Amplitude
    bool noteOn;
    float sample_rate_;
};

