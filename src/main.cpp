#include "RtMidi.h"
#include "synth_engine.h"
#include "portaudio_output.h"
#include <iostream>
#include <thread>
#include <atomic>
#include "midi_handler.h"

std::atomic<bool> running(true);

// MIDI Setup (Allows user to select a MIDI device)
void SetupMidi(SynthEngine &synth, RtMidiIn &midiIn)
{
    unsigned int numPorts = midiIn.getPortCount();
    if (numPorts == 0)
    {
        std::cerr << "No MIDI devices found." << std::endl;
        return;
    }

    std::cout << "Available MIDI Devices:\n";
    for (unsigned int i = 0; i < numPorts; i++)
    {
        std::cout << i << ": " << midiIn.getPortName(i) << std::endl;
    }

    int deviceID = 0;
    std::cout << "Select MIDI device ID (default 0): ";
    std::cin >> deviceID;
    if (deviceID >= numPorts) deviceID = 0;

    std::cout << "Opening MIDI device: " << midiIn.getPortName(deviceID) << std::endl;
    midiIn.openPort(deviceID);
    midiIn.setCallback(&MidiCallback, &synth);
    midiIn.ignoreTypes(false, false, true);
}

// Main Loop (Handles Audio and MIDI Together)
int main(int argc, char *argv[])
{
    std::string config_file = (argc > 1) ? argv[1] : "config.yml";
    SynthEngine synth;
    synth.Init(44100.0f);

    RtMidiIn midiIn;

    // ✅ Run MIDI handling in a separate thread
    std::thread midiThread([&]() { SetupMidi(synth, midiIn); });

    // ✅ Run PortAudio in the main thread
    StartAudio(&synth);

    std::cout << "MIDI synth ready! Press 'q' to quit.\n";

    // Wait for user input
    while (running)
    {
        if (std::cin.get() == 'q')
        {
            std::cout << "quitting...\n";
            running = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Clean up
    midiIn.closePort();
    StopAudio();  // Ensure PortAudio stops
    midiThread.join();

    std::cout << "Exiting cleanly.\n";
    return 0;
}


