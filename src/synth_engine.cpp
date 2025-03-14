#include "synth_engine.h"

SynthEngine::SynthEngine(float sample_rate, int max_voices)
    : sample_rate_(sample_rate), max_voices_(max_voices)
{
    voices_.resize(max_voices_); // Ensure voices_ is properly initialized
    for (auto &voice : voices_)
    {
        voice.osc.Init(sample_rate_);
        voice.osc.SetWaveform(current_waveform_);
        voice.active = false;
    }
    InitEnvelopes(); // Now safe to call!
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

            if (!voice.env.IsRunning()) // Envelope has fully released
            {
                voice.active = false; // Deactivate the voice
                voice.freq = 0.0f; // Reset frequency
            }
        }
    }
    return output / max_voices_;
}



