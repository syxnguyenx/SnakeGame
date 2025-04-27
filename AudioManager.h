#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <SDL_mixer.h>
#include <string>
#include <map>

class AudioManager {
public:
    static AudioManager* Instance();

    bool loadSound(const std::string& filePath, const std::string& id);
    bool loadMusic(const std::string& filePath, const std::string& id);
    void playSound(const std::string& id, int loop = 0);
    void playMusic(const std::string& id, int loop = -1);
    void setSoundVolume(int volume);
    void setMusicVolume(int volume);

private:
    AudioManager();
    static AudioManager* s_pInstance;
    std::map<std::string, Mix_Chunk*> m_sfxs;
    std::map<std::string, Mix_Music*> m_music;
};

#endif
