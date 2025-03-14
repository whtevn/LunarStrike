#include "synth_engine.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <termios.h>
#include <unistd.h>
#include <sys/stat.h>

std::atomic<bool> running(true);
std::atomic<int> current_waveform(Oscillator::WAVE_SIN);
std::string config_file = "config.yml";
time_t last_modified = 0;

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

bool FileModified(const std::string &filename)
{
    struct stat file_stat;
    if (stat(filename.c_str(), &file_stat) == 0)
    {
        if (file_stat.st_mtime > last_modified)
        {
            last_modified = file_stat.st_mtime;
            return true;
        }
    }
    return false;
}

void WatchConfig(SynthEngine &synth)
{
    while (running)
    {
        if (FileModified(config_file))
        {
            synth.ReloadConfig();
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
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

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <config.yml>\n";
        return 1;
    }

    config_file = argv[1];
    SynthEngine synth(44100.0f, 8, config_file);
    std::thread config_watcher(WatchConfig, std::ref(synth));
    config_watcher.detach();

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
        if (ch == 'z') synth.SetDelayTime(0.3f);
        if (ch == 'x') synth.SetDelayTime(0.6f);
        if (ch == 'c') synth.SetDelayFeedback(0.3f);
        if (ch == 'v') synth.SetDelayFeedback(0.7f);
        if (ch == 'b') synth.SetOverdrive(0.3f);
        if (ch == 'n') synth.SetOverdrive(0.8f);

        if (ch == 'q') synth.NoteOn(440.0f);
        if (ch == 'e') synth.NoteOff(440.0f);
        if (ch == '1') current_waveform.store(Oscillator::WAVE_SIN);
        if (ch == '2') current_waveform.store(Oscillator::WAVE_SQUARE);
        if (ch == '3') current_waveform.store(Oscillator::WAVE_SAW);
        if (ch == 'p') running = false;
    }

    synth_thread.join();
    return 0;
}

