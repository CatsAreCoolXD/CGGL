#include "CGGL/audio.h"

#define MINIAUDIO_IMPLEMENTATION
#include "CGGL/miniaudio.h"

#include <iostream>
#include <algorithm>
#include <vector>

namespace cg {
    namespace {
        ma_result result;
        ma_engine engine;
        std::vector<Sound*> audioBuffer;
        std::vector<Sound*> originalAudioBuffer;

        bool ready = false;

        int sounds = 0;
    }

    void InitSound(){
        result = ma_engine_init(NULL, &engine);
        if (result != MA_SUCCESS) {
            std::cout << "Failed to initiliaze audio player!\n";
            return;
        }

        ready = true;
    }

    void Play(Sound *sound, float volume){
        // Make sure engine is ready
        if (!ready){
            cg::InitSound();
            if (!ready) return;
        }

        // Create a copy of the sound which will allow multiple of the same sound to play at once
        cg::Sound* newSound = new Sound(sound->path, true);
        newSound->index = sound->index;

        ma_sound_set_volume(&newSound->sound, volume);
        ma_sound_start(&newSound->sound);

        // Add the sound to the audio buffer
        audioBuffer.push_back(newSound);
    }

    void Stop(Sound *sound)
    {
        // Todo: find a better solution for this
        ma_sound_set_volume(&sound->sound, 0.f);
    }

    void ToggleMute()
    {
        static bool muted = false;
        muted = !muted;
        ma_engine_set_volume(&engine, 1.f * !muted);
    }

    bool IsPlaying(Sound *sound)
    {
        // If the sound is in the audio buffer, it means its still playing
        for (Sound* s : audioBuffer){
            if (sound->index == s->index) return true;
        }
        return false;
    }

    void ShutdownAudio(){
        for (Sound* sound : audioBuffer){
            ma_sound_uninit(&sound->sound);
            delete sound;
        }

        for (Sound* sound : originalAudioBuffer){
            ma_sound_uninit(&sound->sound);
            delete sound;
        }

        ma_engine_uninit(&engine);
    }

    void CleanAudioBuffer()
    {
        std::vector<int> toRemoveaudioBuffer;

        // Remove sounds that are finished playing
        for (int i = 0; i < audioBuffer.size(); i++){
            Sound* sound = audioBuffer[i];

            if (!cg::IsPlaying(sound)){
                toRemoveaudioBuffer.push_back(i);
            }
        }

        if (toRemoveaudioBuffer.empty()) return;

        for (int i = toRemoveaudioBuffer.size() - 1; i >= 0; i--){
            ma_sound_uninit(&audioBuffer[i]->sound);
            delete audioBuffer[i];
            audioBuffer.erase(audioBuffer.begin() + i);
        }
    }

    Sound::Sound(std::string path, bool isClone)
    {
        // Make sure engine is ready
        if (!ready){
            cg::InitSound();
            if (!ready) return;
        }

        this->path = path;
        result = ma_sound_init_from_file(
            &engine,
            path.c_str(),
            0,
            NULL,
            NULL,
            &sound
        );

        if (result != MA_SUCCESS){
            std::cout << "Couldn't create sound object!\n";
        }

        index = ++sounds;

        if (!isClone)
            originalAudioBuffer.push_back(this);
    }
}