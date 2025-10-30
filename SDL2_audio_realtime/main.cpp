/* 
main.cpp
Andrew Baluyot
Started: 2024-11-15
Updated: 2024-11-22
  
For CS 301 Fall 2024 Class Project (Generating 8-Bit Music)
A C++ file that generates the waveforms associated with 8-Bit (Chiptune) music.
Four waveform types: Sine, Square, Triangle, Sawtooth.
A WaveformType struct can house values for the type of waveform, frequency, and phase.
Utilizing SDL audio capabilities, these structs can be passed to SDL audio functions that
output the sound to the listener.
*/

#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>

// Audio format constants
const int SAMPLE_RATE = 44100; // Standard audio sample rate
const int AMPLITUDE = 30;     // 8-bit range (0-255)

// Waveform type
enum WaveformType { SQUARE, TRIANGLE, SAWTOOTH, SINE };

// Audio state
struct AudioState {
    WaveformType type;
    double frequency;
    double phase;
};

// Function to generate samples for different waveforms
Uint8 generateSample(WaveformType type, double frequency, double& phase) {
    double increment = frequency / SAMPLE_RATE; // Phase increment per sample
    double value = 0;

    switch (type) {
        case SQUARE:
            if (phase < 0.5) {
                value = AMPLITUDE;
            }
            else value = -AMPLITUDE;
            break;
        case TRIANGLE:
            if (phase < 0.5) {
                value = 4 * AMPLITUDE * phase - AMPLITUDE;
            }
            else value = -4 * AMPLITUDE * (phase - 0.5) + AMPLITUDE;
            break;
        case SAWTOOTH:
            value = 2 * AMPLITUDE * (phase - 0.5);
            break;
        case SINE:
            value = AMPLITUDE * std::sin(2 * M_PI * phase);
            break;
    }

    phase += increment;
    if (phase >= 1.0) phase -= 1.0; // Wrap phase
    return static_cast<Uint8>(value + 128); // Shift to 8-bit range
}

// SDL audio callback
void audioCallback(void* userdata, Uint8* stream, int len) {
    AudioState* state = static_cast<AudioState*>(userdata);
    for (int i = 0; i < len; ++i) {
        stream[i] = generateSample(state->type, state->frequency, state->phase);
    }
}

int main() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    AudioState audioState = {SQUARE, 440.0, 0.0}; // A4, Square wave

    SDL_AudioSpec desiredSpec{};
    desiredSpec.freq = SAMPLE_RATE;
    desiredSpec.format = AUDIO_U8; // 8-bit unsigned audio
    desiredSpec.channels = 1;     // Mono audio
    desiredSpec.samples = 4096;   // Buffer size
    desiredSpec.callback = audioCallback;
    desiredSpec.userdata = &audioState;

    SDL_AudioSpec obtainedSpec{};
    if (SDL_OpenAudio(&desiredSpec, &obtainedSpec) < 0) {
        std::cerr << "Failed to open audio: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_PauseAudio(0); // Start audio playback

    std::cout << "Playing a square wave tone at 440Hz. Press Enter to exit." << std::endl;
    std::cin.get();

    SDL_CloseAudio();
    SDL_Quit();
    return 0;
}
