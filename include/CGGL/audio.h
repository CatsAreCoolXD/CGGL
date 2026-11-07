#ifndef CGGL_AUDIO_H
#define CGGL_AUDIO_H

#include "CGGL/miniaudio.h"

#include <string>

namespace cg {
    class Sound {
        public:
            Sound(std::string path, bool isClone = false);
            Sound() {}
            std::string path;
            ma_sound sound;
            int index;
    };

    void InitSound();

    // Volume:
    // 0.f = 0%
    // 1.f = 100%
    void Play(Sound* sound, float volume = 1.f);
    void Stop(Sound* sound);

    void ToggleMute();

    bool IsPlaying(Sound* sound);

    void ShutdownAudio();

    void CleanAudioBuffer();
}

#endif