#include "RtMidi.h"
#include "synth_engine.h"
#include <iostream>
#include <thread>
#include <atomic>

std::atomic<bool> running(true);

void MidiCallback(double deltatime, std::vector<unsigned char> *message, void *userData)
{
    SynthEngine *synth = static_cast<SynthEngine *>(userData);

    if (message->size() < 2) return; // Ignore malformed messages

    unsigned char status = message->at(0);
    unsigned char data1 = message->at(1);
    unsigned char data2 = (message->size() > 2) ? message->at(2) : 0;

    if ((status & 0xF0) == 0x90 && data2 > 0) // Note On
    {
        synth->ProcessMidiNoteOn(data1, data2);
    }
    else if ((status & 0xF0) == 0x80 || ((status & 0xF0) == 0x90 && data2 == 0)) // Note Off
    {
        synth->ProcessMidiNoteOff(data1);
    }
}

void SetupMidi(SynthEngine &synth)
{
    RtMidiIn midiIn;

    if (midiIn.getPortCount() == 0)
    {
        std::cerr << "No MIDI devices found." << std::endl;
        return;
    }

    std::cout << "Opening MIDI device: " << midiIn.getPortName(0) << std::endl;
    midiIn.openPort(0);
    midiIn.setCallback(&MidiCallback, &synth);
    midiIn.ignoreTypes(false, false, true);

    while (running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    midiIn.closePort();
}

int main(int argc, char *argv[])
{
    std::string config_file = (argc > 1) ? argv[1] : "config.yml";
    SynthEngine synth(44100.0f, 8, config_file);

    std::thread midiThread(SetupMidi, std::ref(synth));

    std::cout << "MIDI synth ready! Press 'q' to quit.\n";

    while (running)
    {
        char ch = getchar();
        if (ch == 'q') running = false;
    }

    midiThread.join();
    return 0;
}

