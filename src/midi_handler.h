#ifndef MIDI_HANDLER_H
#define MIDI_HANDLER_H

#include "synth_engine.h"
#include <vector>

// ✅ Declare `MidiCallback`, but do NOT define it here
void MidiCallback(double deltatime, std::vector<unsigned char> *message, void *userData);

// ✅ Declare `StartMIDI`, but do NOT define it here
void StartMIDI(SynthEngine &synth);

#endif // MIDI_HANDLER_H

