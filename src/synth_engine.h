#pragma once

#include "daisysp.h"
#include <vector>

using namespace daisysp;

class SynthEngine
{
public:
    SynthEngine(float sample_rate, int max_voices = 8);

    void SetWaveform(int waveform); // Set waveform type
    void NoteOn(float freq);        // Trigger a note
    void NoteOff(float freq);       // Stop a note
    void InitEnvelopes();
    float Process();                // Generate audio sample


private:
    struct Voice
    {
        Oscillator osc;
        Adsr env; 
        float freq = 0.0f; 
        bool active = false;
        bool gate = false; 

    };


    float sample_rate_;
    std::vector<Voice> voices_;
    int max_voices_;
    int current_waveform_ = Oscillator::WAVE_SIN;
};

