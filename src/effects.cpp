#include "effects.h"

EffectProcessor::EffectProcessor(float sample_rate) : sample_rate_(sample_rate)
{
    // Initialize effects
    delay.Init();
    delay.SetDelay(sample_rate * 0); // Default: 500ms delay

    overdrive.Init();
    overdrive.SetDrive(0.5f); // Default: Medium overdrive
}

float EffectProcessor::Process(float input)
{
    // Apply overdrive
    float processed_signal = overdrive.Process(input);

    // Apply delay
    float delayed_signal = delay.Read();
    delay.Write((processed_signal + delayed_signal * delay_feedback_));

    return (processed_signal + delayed_signal) / 2.0f; // Mix dry & wet
}

void EffectProcessor::SetDelayTime(float time)
{
    delay.SetDelay(sample_rate_ * time);
}

void EffectProcessor::SetDelayFeedback(float feedback)
{
    delay_feedback_ = feedback;
}

void EffectProcessor::SetOverdrive(float amount)
{
    overdrive.SetDrive(amount);
}

