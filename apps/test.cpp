
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

#include <AL/al.h>
#include <AL/alc.h>

#define STB_VORBIS_IMPLEMENTATION
#include "stb_vorbis.c"   // drop in stb_vorbis.c
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"       // drop in dr_wav.h

// Helper: check for OpenAL errors
void CheckALError(const std::string& msg) {
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "OpenAL error at " << msg << ": " << alGetString(error) << std::endl;
    }
}

// Load a WAV file into an OpenAL buffer
ALuint LoadWavToBuffer(const char* filename) {
    drwav wav;
    if (!drwav_init_file(&wav, filename, nullptr)) {
        throw std::runtime_error("Failed to load WAV file");
    }

    std::vector<int16_t> pcmData(wav.totalPCMFrameCount * wav.channels);
    drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, pcmData.data());
    drwav_uninit(&wav);

    ALenum format = (wav.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, pcmData.data(),
                 pcmData.size() * sizeof(int16_t), wav.sampleRate);
    CheckALError("alBufferData(WAV)");

    return buffer;
}

// Load an OGG file into an OpenAL buffer (non-streaming example)
ALuint LoadOggToBuffer(const char* filename) {
    int channels, sampleRate;
    short* output;
    int samples = stb_vorbis_decode_filename(filename, &channels, &sampleRate, &output);
    if (samples < 0) {
        throw std::runtime_error("Failed to load OGG file");
    }

    ALenum format = (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, output,
                 samples * channels * sizeof(short), sampleRate);
    free(output);
    CheckALError("alBufferData(OGG)");

    return buffer;
}

int main() {
    // ---- Init OpenAL ----
    ALCdevice* device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "Failed to open OpenAL device\n";
        return -1;
    }

    ALCcontext* context = alcCreateContext(device, nullptr);
    alcMakeContextCurrent(context);

    try {
        // ---- Load sounds ----
        ALuint bgBuffer = LoadOggToBuffer("./res/sounds/cave.ogg");
        ALuint clickBuffer = LoadWavToBuffer("./res/sounds/click.wav");

        ALuint bgSource, clickSource;
        alGenSources(1, &bgSource);
        alGenSources(1, &clickSource);

        alSourcei(bgSource, AL_BUFFER, bgBuffer);
        alSourcei(bgSource, AL_LOOPING, AL_TRUE);

        alSourcei(clickSource, AL_BUFFER, clickBuffer);
        alSourcei(clickSource, AL_LOOPING, AL_FALSE);

        // ---- Play background music ----
        alSourcePlay(bgSource);
        std::cout << "Background music playing...\n";

        // ---- Clicks on a separate thread ----
        std::thread clickThread([&]() {
            for (int i = 0; i < 5; i++) {
                std::this_thread::sleep_for(std::chrono::seconds(2));
                std::cout << "Click!\n";
                alSourcePlay(clickSource);
            }
            });
        clickThread.detach();

        // ---- Keep main thread alive for music & clicks ----
        std::this_thread::sleep_for(std::chrono::seconds(12));

        // ---- Cleanup ----
        alDeleteSources(1, &bgSource);
        alDeleteSources(1, &clickSource);
        alDeleteBuffers(1, &bgBuffer);
        alDeleteBuffers(1, &clickBuffer);

        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        alcCloseDevice(device);

        std::cout << "Done.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Audio exception: " << e.what() << std::endl;
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        alcCloseDevice(device);
        return -1;
    }

    return 0;
}
