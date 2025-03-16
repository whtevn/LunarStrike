#include "portaudio_output.h"
#include <portaudio.h>
#include <iostream>

static PaStream *stream = nullptr;  // Ensure it's globally accessible

// Audio callback function
static int AudioCallback(const void *input, void *output,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo *timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData) {
    SynthEngine *synth = static_cast<SynthEngine *>(userData);
    float *out = (float *)output;

    for (unsigned int i = 0; i < framesPerBuffer; i++) {
        *out++ = synth->Process();  // Left Channel
        *out++ = synth->Process();  // Right Channel
    }
    return 0;
}

// Start PortAudio and set up the audio stream
void StartAudio(SynthEngine *synth) {
    Pa_Initialize();

    Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, 44100, 512, AudioCallback, synth);
    Pa_StartStream(stream);

    std::cout << "Audio started with PortAudio!\n";
}


// Stop PortAudio cleanly
void StopAudio() {
    if (stream) {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        stream = nullptr;  // Reset stream pointer
    }
    Pa_Terminate();
    std::cout << "Audio stopped.\n";
}

