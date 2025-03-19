#pragma once
#include "daisysp.h"

using namespace daisysp;

class SynthEngine {
public:
    void Init(float sample_rate);
    float Process();

    // MIDI Handlers
    void OnNoteOn(uint8_t note, uint8_t velocity);
    void OnNoteOff();

    // LFO Modulation
    void SetLfoDepth(float depth);
    void SetLfoTarget(int target);

private:
    Oscillator osc;
    Oscillator lfo;
    Adsr envelope;
    float frequency;
    float lfo_depth;
    int lfo_target; // 0: Pitch, 1: Amplitude
    bool noteOn;
};

