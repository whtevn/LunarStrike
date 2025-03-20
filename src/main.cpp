#include "RtMidi.h"
#include "synth_engine.h"
#include "portaudio_output.h"
#include <iostream>
#include <thread>
#include <atomic>
#include "midi_handler.h"
#include <unistd.h>
#include <ncurses.h>
#include <unordered_map>  // Fixes 'unordered_map' error
#include <sys/time.h>     // Fixes 'struct timeval' error
#include <sys/select.h>   // Fixes 'fd_set' error

std::atomic<bool> running(true);


static std::unordered_map<int, bool> keyStates; // ✅ Track key states
static bool kToggled = false; // ✅ Track 'K' key toggle state

void KeypressLoop(SynthEngine &synth)
{
    initscr();            // Initialize ncurses mode
    cbreak();             // Disable line buffering
    noecho();             // Don't echo input characters
    timeout(10);          // ✅ Prevents key repeat interference
    keypad(stdscr, TRUE); // Enable function keys
    curs_set(0);          // Hide cursor

    while (running)
    {
        int ch = getch(); // Non-blocking key press check

        if (ch != ERR) // A key was pressed
        {
            // 🎹 'J' key → Press triggers ON, Release triggers OFF
            if (ch == 'j' && keyStates.find(ch) == keyStates.end()) // First press only
            {
                printw("J Key - Note On\n");
                refresh();
                synth.OnNoteOn(60, 127); // Middle C
                keyStates[ch] = true; // Mark key as held
            }

            // 🎹 'K' key → Toggle ON/OFF every press
            else if (ch == 'k' && keyStates.find(ch) == keyStates.end()) // First press only
            {
                if (!kToggled) // First press → ON
                {
                    printw("K Key - Note On\n");
                    refresh();
                    synth.OnNoteOn(60, 127);
                }
                else // Second press → OFF
                {
                    printw("K Key - Note Off\n");
                    refresh();
                    synth.OnNoteOff();
                }
                kToggled = !kToggled; // Flip toggle state
                keyStates[ch] = true; // Mark key as held
            }

            // 🎹 Quit on 'q'
            else if (ch == 'q')
            {
                printw("Quitting...\n");
                refresh();
                running = false;
            }
        }
        else // No key currently pressed → Detect release
        {
            if (keyStates.find('j') != keyStates.end()) // ✅ 'J' key was previously held, now released
            {
                printw("J Key - Note Off\n");
                refresh();
                synth.OnNoteOff();
                keyStates.erase('j'); // ✅ Remove key from state map
            }

            if (keyStates.find('k') != keyStates.end()) // ✅ 'K' key release detection
            {
                keyStates.erase('k'); // ✅ Allow 'K' key to trigger again
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    endwin(); // Restore normal terminal behavior
}




// Check if a key was pressed
bool KeyPressed()
{
    struct timeval timeout = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &timeout) > 0;
}

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

// Main Loop (Handles Audio, MIDI, and Key Input)
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

    std::cout << "MIDI synth ready! Press 'q' to quit, spacebar to play a note.\n";

    // ✅ Run Keypress Listener in a Separate Thread
    std::thread keyThread([&]() { KeypressLoop(synth); });

    // Wait for user input
    midiThread.join();
    keyThread.join();

    // Clean up
    midiIn.closePort();
    StopAudio(); // Ensure PortAudio stops

    std::cout << "Exiting cleanly.\n";
    return 0;
}

