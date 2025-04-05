#include "synth_engine.h"
#include <cmath>
#include <iostream>
#include <ncurses.h>

void SynthEngine::Init(float sample_rate) {
    sample_rate_ = sample_rate;
    float attack = 0.1;
    float decay = 0.2;
    float sustain = 0.3;
    float release = 0.4;

    osc.Init(sample_rate);
    osc.SetWaveform(Oscillator::WAVE_SIN);
    osc.SetAmp(1.0f);

    lfo.Init(sample_rate);
    lfo.SetWaveform(Oscillator::WAVE_TRI);
    lfo.SetFreq(0.25f); // Default slow LFO

    SetEnvelopeParameters(attack, decay, sustain, release);

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

    // Log envelope amplitude every ~0.1 seconds (at 44100 samples/sec) using ncurses printw
    static int sampleCounter = 0;
    if (++sampleCounter % 4410 == 0) {
        // Move cursor to row 0, column 0 and print envelope amplitude
        move(0, 0);
        printw("Envelope Amplitude: %.4f, noteOn: %d    ", envAmp, noteOn);
        refresh();
    }

    return osc.Process() * envAmp;
}


/*
float SynthEngine::Process() {
    float lfo_val = lfo.Process() * lfo_depth;
    float freq_mod = (lfo_target == 0) ? lfo_val * 5.0f : 0.0f; // Vibrato
    float amp_mod = (lfo_target == 1) ? lfo_val : 0.0f; // Tremolo

    osc.SetFreq(frequency + freq_mod);
    float envAmp = envelope.Process(noteOn) * (1.0f + amp_mod);
    return osc.Process() * envAmp;
}
*/

void SynthEngine::OnNoteOn(uint8_t note, uint8_t velocity) {
    frequency = 440.0f * powf(2.0f, (note - 69) / 12.0f);
    noteOn = true;
}

void SynthEngine::OnNoteOff() {
    noteOn = false;
}

void SynthEngine::SetLfoDepth(float depth) {
    lfo_depth = depth;
}

void SynthEngine::SetLfoTarget(int target) {
    lfo_target = target; // 0 = Pitch, 1 = Amplitude
}

void SynthEngine::SetEnvelopeParameters(float attack, float decay, float sustain, float release) {
    envelope.Init(sample_rate_);
    envelope.SetSustainLevel(sustain);
    envelope.SetAttackTime(attack, 0.5f);
    envelope.SetTime(ADSR_SEG_DECAY, decay);
    envelope.SetTime(ADSR_SEG_RELEASE, release);
}
