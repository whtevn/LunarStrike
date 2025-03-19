#include "synth_engine.h"
#include <cmath>

void SynthEngine::Init(float sample_rate) {
    osc.Init(sample_rate);
    osc.SetWaveform(Oscillator::WAVE_SIN);
    osc.SetAmp(1.0f);

    lfo.Init(sample_rate);
    lfo.SetWaveform(Oscillator::WAVE_TRI);
    lfo.SetFreq(0.25f); // Default slow LFO

    envelope.Init(sample_rate);
    envelope.SetTime(ADSR_SEG_ATTACK, 0.1f);
    envelope.SetTime(ADSR_SEG_DECAY, 0.3f);
    envelope.SetSustainLevel(0.8f);
    envelope.SetTime(ADSR_SEG_RELEASE, 0.5f);

    frequency = 440.0f;
    noteOn = false;
    lfo_depth = 0.0f;
    lfo_target = 0;
}

float SynthEngine::Process() {
    float lfo_val = lfo.Process() * lfo_depth;
    float freq_mod = (lfo_target == 0) ? lfo_val * 5.0f : 0.0f; // Vibrato
    float amp_mod = (lfo_target == 1) ? lfo_val : 0.0f; // Tremolo

    osc.SetFreq(frequency + freq_mod);
    float envAmp = envelope.Process(noteOn) * (1.0f + amp_mod);
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

void SynthEngine::SetLfoDepth(float depth) {
    lfo_depth = depth;
}

void SynthEngine::SetLfoTarget(int target) {
    lfo_target = target; // 0 = Pitch, 1 = Amplitude
}

