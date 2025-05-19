#include "AudioManager.h"
#include <SDL_mixer.h>
#include <iostream>

AudioManager* AudioManager::s_pInstance = nullptr;

AudioManager* AudioManager::Instance() {
    if(s_pInstance == nullptr) {
        s_pInstance = new AudioManager();
    }
    return s_pInstance;
}



bool AudioManager::loadSound(const std::string& filePath, const std::string& id) {
    Mix_Chunk* pChunk = Mix_LoadWAV(filePath.c_str());
    if(pChunk == nullptr) {
        return false;
    }
    m_sfxs[id] = pChunk;
    return true;
}

AudioManager::AudioManager() : m_isMuted(false), m_currentMusicVolumePercent(80), m_currentSfxVolumePercent(80),
                              m_musicVolumeBeforeMute(80 * MIX_MAX_VOLUME / 100),
                              m_sfxVolumeBeforeMute_chunkSpecific(80 * MIX_MAX_VOLUME / 100) // Giá trị mặc định
{
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
}

bool AudioManager::loadMusic(const std::string& filePath, const std::string& id) {
    Mix_Music* pMusic = Mix_LoadMUS(filePath.c_str());
    if(pMusic == nullptr) {
        return false;
    }
    m_music[id] = pMusic;
    return true;
}

void AudioManager::setGlobalVolume(int musicVolPercent, int sfxVolPercent) {
    m_currentMusicVolumePercent = std::max(0, std::min(100, musicVolPercent));
    m_currentSfxVolumePercent = std::max(0, std::min(100, sfxVolPercent));

    if (!m_isMuted) {
        Mix_VolumeMusic(m_currentMusicVolumePercent * MIX_MAX_VOLUME / 100);
    }
}

void AudioManager::toggleMuteAll() {
    m_isMuted = !m_isMuted;
    if (m_isMuted) {
        // Lưu mức âm lượng hiện tại (quy đổi từ phần trăm)
        m_musicVolumeBeforeMute = Mix_VolumeMusic(-1); // Lấy volume hiện tại
        Mix_VolumeMusic(0);
        for (auto& sfx_pair : m_sfxs) {
            if (sfx_pair.second) {
                // Không có cách lấy volume của từng chunk, nên sẽ khó khôi phục chính xác từng cái
                // Thay vào đó, khi unmute, ta sẽ đặt lại volume theo m_currentSfxVolumePercent
                Mix_VolumeChunk(sfx_pair.second, 0);
            }
        }
        std::cout << "Sound Muted" << std::endl;
    } else {
        Mix_VolumeMusic(m_currentMusicVolumePercent * MIX_MAX_VOLUME / 100);
        for (auto& sfx_pair : m_sfxs) {
            if (sfx_pair.second) {
                Mix_VolumeChunk(sfx_pair.second, m_currentSfxVolumePercent * MIX_MAX_VOLUME / 100);
            }
        }
        std::cout << "Sound Unmuted" << std::endl;
    }
}
bool AudioManager::isMuted() const {
    return m_isMuted;
}

void AudioManager::playSound(const std::string& id, int loop) {
    if (m_sfxs.count(id) && m_sfxs[id] != nullptr) {
        if (!m_isMuted) { // Chỉ phát nếu không bị mute
             // Đặt âm lượng cho chunk này dựa trên cài đặt SFX hiện tại trước khi phát
             Mix_VolumeChunk(m_sfxs[id], m_currentSfxVolumePercent * MIX_MAX_VOLUME / 100);
             Mix_PlayChannel(-1, m_sfxs[id], loop);
        }
    } else {
        std::cerr << "Sound with id '" << id << "' not found or is null." << std::endl;
    }
}

 // Sửa lại setMusicVolume để cập nhật cả m_currentMusicVolumePercent
 void AudioManager::setMusicVolume(int volume) { // volume ở đây là 0-MIX_MAX_VOLUME
     m_currentMusicVolumePercent = (volume * 100) / MIX_MAX_VOLUME;
     if (!m_isMuted) {
         Mix_VolumeMusic(volume);
     }
 }

 // Sửa lại setSoundVolume để cập nhật m_currentSfxVolumePercent và áp dụng cho tất cả sounds
 void AudioManager::setSoundVolume(int volume) { // volume ở đây là 0-MIX_MAX_VOLUME
     m_currentSfxVolumePercent = (volume * 100) / MIX_MAX_VOLUME;
     if (!m_isMuted) {
         for (auto& sfx : m_sfxs) {
             if (sfx.second) {
                 Mix_VolumeChunk(sfx.second, volume);
             }
         }
     }
 }
 void AudioManager::playMusic(const std::string& id, int loop) {
    // Kiểm tra xem music ID có tồn tại trong map không
    auto it = m_music.find(id);
    if (it != m_music.end() && it->second != nullptr) {
        // Kiểm tra xem có đang phát nhạc không, nếu có thì dừng trước khi phát nhạc mới (tùy chọn)
        if (Mix_PlayingMusic()) {
            Mix_HaltMusic(); // Dừng nhạc đang phát
        }
        // Phát nhạc mới
        if (Mix_PlayMusic(it->second, loop) == -1) {
            std::cerr << "Loi: Khong the phat music voi ID '" << id << "'. SDL_mixer Error: " << Mix_GetError() << std::endl;
        }
    } else {
        std::cerr << "Loi: Khong tim thay music voi ID '" << id << "' trong AudioManager." << std::endl;
    }
}
