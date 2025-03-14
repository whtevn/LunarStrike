#include "synth_engine.h"
#include <iostream>


SynthEngine::SynthEngine(float sample_rate, int max_voices, const std::string &config_file)
    : sample_rate_(sample_rate), max_voices_(max_voices), config_(config_file), effects_(sample_rate)
{
    voices_.resize(max_voices_);
    for (auto &voice : voices_)
    {
        voice.osc.Init(sample_rate_);
        voice.active = false;
    }

    ReloadConfig(); // Load settings at startup
}


void SynthEngine::ReloadConfig()
{
    config_.Load();

    // Apply ADSR settings
    for (auto &voice : voices_)
    {
        voice.env.SetTime(ADSR_SEG_ATTACK, config_.GetADSR("attack"));
        voice.env.SetTime(ADSR_SEG_DECAY, config_.GetADSR("decay"));
        voice.env.SetSustainLevel(config_.GetADSR("sustain"));
        voice.env.SetTime(ADSR_SEG_RELEASE, config_.GetADSR("release"));
    }

    // Apply effect settings
    effects_.SetDelayTime(config_.GetEffect("delay_time"));
    effects_.SetDelayFeedback(config_.GetEffect("delay_feedback"));
    effects_.SetOverdrive(config_.GetEffect("overdrive"));

    std::cout << "Config reloaded!" << std::endl;
}




void SynthEngine::InitEnvelopes()
{
    for (auto &voice : voices_)
    {
        voice.env.Init(sample_rate_);
        voice.env.SetTime(ADSR_SEG_ATTACK, 0.0f);  // Attack: 100ms
        voice.env.SetTime(ADSR_SEG_DECAY, 0.1f);   // Decay: 100ms
        voice.env.SetSustainLevel(0.7f);           // Sustain: 70%
        voice.env.SetTime(ADSR_SEG_RELEASE, 0.2f); // Release: 200ms
    }
}


void SynthEngine::SetWaveform(int waveform)
{
    current_waveform_ = waveform;
    for (auto &voice : voices_)
    {
        voice.osc.SetWaveform(waveform);
    }
}
void SynthEngine::NoteOn(float freq)
{
    for (auto &voice : voices_)
    {
        if (!voice.active)
        {
            voice.osc.SetFreq(freq);
            voice.freq = freq;
            voice.active = true;
            voice.gate = true;
            voice.env.Retrigger(true); // Pass `true` for hard retrigger

            return;
        }
    }
}

void SynthEngine::NoteOff(float freq)
{
    for (auto &voice : voices_)
    {
        if (voice.active && voice.freq == freq)
        {
            voice.gate = false;
            return;
        }
    }
}

float SynthEngine::Process()
{
    float output = 0.0f;
    for (auto &voice : voices_)
    {
        if (voice.active)
        {
            float amp = voice.env.Process(voice.gate);
            output += voice.osc.Process() * amp;
        }
    }

    return effects_.Process(output); // Apply effects after synthesis
}


// Implement missing functions
void SynthEngine::SetDelayTime(float time)
{
    effects_.SetDelayTime(time);
}

void SynthEngine::SetDelayFeedback(float feedback)
{
    effects_.SetDelayFeedback(feedback);
}

void SynthEngine::SetOverdrive(float amount)
{
    effects_.SetOverdrive(amount);
}





