#pragma once
#include "daisysp.h"

using namespace daisysp;

class EffectProcessor
{
public:
    EffectProcessor(float sample_rate);
    float Process(float input);

    void SetDelayTime(float time);
    void SetDelayFeedback(float feedback);
    void SetOverdrive(float amount);

private:
    DelayLine<float, 96000> delay;
    Overdrive overdrive;
    float delay_feedback_ = 0.5f;
    float sample_rate_;
};

