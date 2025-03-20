#include "midi_handler.h"
#include "RtMidi.h"
#include <iostream>
#include <thread>
#include <chrono>

#ifdef MACOS
  #define MIDI_API RtMidi::Api::MACOSX_CORE  
#else
  #define MIDI_API RtMidi::Api::LINUX_ALSA  
#endif

RtMidiIn *midiin = nullptr;


void MidiCallback(double deltatime, std::vector<unsigned char> *message, void *userData) {
    SynthEngine *synth = static_cast<SynthEngine *>(userData);

    if (message->size() < 2) return; // Ignore malformed messages

    unsigned char status = message->at(0);
    unsigned char data1 = message->at(1);
    unsigned char data2 = (message->size() > 2) ? message->at(2) : 0;

    if ((status & 0xF0) == 0x90 && data2 > 0) {  // Note On
        synth->OnNoteOn(data1, data2);
    } else if ((status & 0xF0) == 0x80 || ((status & 0xF0) == 0x90 && data2 == 0)) {  // Note Off
        synth->OnNoteOff();
    }
}

void StartMIDI(SynthEngine &synth) {
    RtMidiIn midi;

    if (midi.getPortCount() == 0) {
        std::cerr << "No MIDI devices found." << std::endl;
        return;
    }

    std::cout << "Opening MIDI device: " << midi.getPortName(0) << std::endl;
    midi.openPort(0);
    midi.setCallback(&MidiCallback, &synth);
    midi.ignoreTypes(false, false, true);

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

