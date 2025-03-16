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

private:
    Oscillator osc;
    Adsr envelope;
    float frequency;
    bool noteOn;
};

