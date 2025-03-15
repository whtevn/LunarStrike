#pragma once
#include "effects.h"
#include "daisysp.h"
#include "config_loader.h"
#include <vector>

class SynthEngine
{
public:
    SynthEngine(float sample_rate, int max_voices = 8, const std::string &config_file = "config.yml");


    void ProcessMidiNoteOn(int note, int velocity);
    void ProcessMidiNoteOff(int note);

    void SetWaveform(int waveform);
    void NoteOn(float freq);
    void NoteOff(float freq);
    float Process();

    void SetDelayTime(float time);
    void SetDelayFeedback(float feedback);
    void SetOverdrive(float amount);
    void ReloadConfig();

private:
    void InitEnvelopes(); // ✅ Declare InitEnvelopes here

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

    EffectProcessor effects_;
    ConfigLoader config_;

};

