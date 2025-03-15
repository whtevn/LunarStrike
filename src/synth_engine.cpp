#include "synth_engine.h"
#include <iostream>
#include <cmath>

SynthEngine::SynthEngine(float sample_rate, int max_voices, const std::string &config_file)
    : sample_rate_(sample_rate), max_voices_(max_voices), config_(config_file), effects_(sample_rate)
{
    voices_.resize(max_voices_);
    for (auto &voice : voices_)
    {
        voice.osc.Init(sample_rate_);
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

void SynthEngine::SetWaveform(int waveform)
{
    current_waveform_ = waveform;
    for (auto &voice : voices_)
    {
        voice.osc.SetWaveform(waveform);
    }
}

float SynthEngine::Process()
{
    float mix = 0.0f;
    for (auto &voice : voices_)
    {
        float amp = voice.env.Process(voice.gate);

        // ✅ If the envelope has finished, fully release the voice
        if (!voice.env.IsRunning())
        {
            voice.freq = 0.0f;  // ✅ Mark this voice as available
            voice.gate = false; // ✅ Ensure it's fully off
        }

        mix += voice.osc.Process() * amp;
    }

    return effects_.Process(mix);
}


void SynthEngine::ProcessMidiNoteOn(int note, int velocity)
{
    float freq = 440.0f * pow(2.0f, (note - 69) / 12.0f);
    float velocity_scaled = velocity / 127.0f;

    std::cout << "SynthEngine: Note On - " << note << " (Freq: " << freq << " Hz, Velocity: " << velocity_scaled << ")\n";

    // 1️⃣ **Find a completely free voice**
    for (auto &voice : voices_)
    {
        if (voice.freq == 0.0f)  // ✅ This voice is available
        {
            voice.osc.SetFreq(freq);
            voice.freq = freq;
            voice.gate = true;
            voice.env.Retrigger(true);
            std::cout << "ACTIVATE (New Voice)\n";
            return;
        }
    }

    // 2️⃣ **No free voices—steal the quietest**
    Voice* quietest_voice = nullptr;
    float min_amp = 1.0f;

    for (auto &voice : voices_)
    {
        float amp = voice.env.Process(voice.gate);
        if (amp < min_amp)
        {
            min_amp = amp;
            quietest_voice = &voice;
        }
    }

    if (quietest_voice)
    {
        std::cout << "No free voices! Stealing the quietest voice.\n";
        quietest_voice->osc.SetFreq(freq);
        quietest_voice->freq = freq;
        quietest_voice->gate = true;
        quietest_voice->env.Retrigger(true);
    }
}


void SynthEngine::ProcessMidiNoteOff(int note)
{
    float freq = 440.0f * pow(2.0f, (note - 69) / 12.0f);
    std::cout << "SynthEngine: Note Off - " << note << " (Freq: " << freq << " Hz)\n";

    for (auto &voice : voices_)
    {
        if (voice.freq == freq)
        {
            voice.gate = false;  // ✅ ADSR handles release
            std::cout << "Release Triggered for " << freq << " Hz\n";
            return;
        }
    }
}



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

