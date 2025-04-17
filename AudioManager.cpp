#include "AudioManager.h"

AudioManager* AudioManager::s_pInstance = nullptr;

AudioManager* AudioManager::Instance() {
    if(s_pInstance == nullptr) {
        s_pInstance = new AudioManager();
    }
    return s_pInstance;
}

AudioManager::AudioManager() {
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
}

bool AudioManager::loadSound(const std::string& filePath, const std::string& id) {
    Mix_Chunk* pChunk = Mix_LoadWAV(filePath.c_str());
    if(pChunk == nullptr) {
        return false;
    }
    m_sfxs[id] = pChunk;
    return true;
}

bool AudioManager::loadMusic(const std::string& filePath, const std::string& id) {
    Mix_Music* pMusic = Mix_LoadMUS(filePath.c_str());
    if(pMusic == nullptr) {
        return false;
    }
    m_music[id] = pMusic;
    return true;
}

void AudioManager::playSound(const std::string& id, int loop) {
    Mix_PlayChannel(-1, m_sfxs[id], loop);
}

void AudioManager::playMusic(const std::string& id, int loop) {
    Mix_PlayMusic(m_music[id], loop);
}

void AudioManager::setSoundVolume(int volume) {
    for(auto& sfx : m_sfxs) {
        Mix_VolumeChunk(sfx.second, volume);
    }
}

void AudioManager::setMusicVolume(int volume) {
    Mix_VolumeMusic(volume);
}
