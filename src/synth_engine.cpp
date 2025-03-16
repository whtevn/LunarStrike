#include "synth_engine.h"
#include <cmath>

void SynthEngine::Init(float sample_rate) {
    osc.Init(sample_rate);
    osc.SetWaveform(Oscillator::WAVE_SAW);
    osc.SetAmp(1.0f);

    envelope.Init(sample_rate);
    envelope.SetTime(ADSR_SEG_ATTACK, 0.01f);
    envelope.SetTime(ADSR_SEG_DECAY, 0.1f);
    envelope.SetSustainLevel(0.7f);
    envelope.SetTime(ADSR_SEG_RELEASE, 0.2f);

    frequency = 440.0f;
    noteOn = false;
}

float SynthEngine::Process() {
    osc.SetFreq(frequency);
    float envAmp = envelope.Process(noteOn);
    return osc.Process() * envAmp;
}

void SynthEngine::OnNoteOn(uint8_t note, uint8_t velocity) {
    frequency = 440.0f * powf(2.0f, (note - 69) / 12.0f);
    noteOn = true;
    envelope.Process(true);
}

void SynthEngine::OnNoteOff() {
    noteOn = false;
    envelope.Process(false);
}

