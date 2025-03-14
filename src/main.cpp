#include "synth_engine.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <termios.h>
#include <unistd.h>

std::atomic<bool> running(true);
std::atomic<int> current_waveform(Oscillator::WAVE_SIN);

char getch()
{
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void synthLoop(SynthEngine &synth)
{
    constexpr int buffer_size = 32;
    FILE *pipe = popen("play -r 44100 -t f32 -q -c 1 -", "w");

    if (!pipe)
    {
        std::cerr << "Failed to open audio stream.\n";
        return;
    }

    float buffer[buffer_size];

    while (running)
    {
        synth.SetWaveform(current_waveform.load());

        for (int i = 0; i < buffer_size; i++)
        {
            buffer[i] = synth.Process();
        }

        fwrite(buffer, sizeof(float), buffer_size, pipe);
        fflush(pipe);
    }

    pclose(pipe);
}

int main()
{
    SynthEngine synth(44100.0f, 8); // 8-voice polyphony

    std::cout << "Controls:\n";
    std::cout << "  Q/E: Play notes (up/down)\n";
    std::cout << "  1: Sine Wave\n";
    std::cout << "  2: Square Wave\n";
    std::cout << "  3: Saw Wave\n";
    std::cout << "  X: Exit\n";

    std::thread synth_thread(synthLoop, std::ref(synth));

    while (running)
    {
        char ch = getch();
        std::cout << "Pressed Key\n";
        if (ch == 'q') synth.NoteOn(440.0f);
        if (ch == 'e') synth.NoteOff(440.0f);
        if (ch == '1') current_waveform.store(Oscillator::WAVE_SIN);
        if (ch == '2') current_waveform.store(Oscillator::WAVE_SQUARE);
        if (ch == '3') current_waveform.store(Oscillator::WAVE_SAW);
        if (ch == 'x') running = false;
    }

    synth_thread.join();
    return 0;
}

