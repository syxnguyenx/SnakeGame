#ifndef AUDIOMANAGER_H_INCLUDED
#define AUDIOMANAGER_H_INCLUDED

#include <SDL_mixer.h>
#include <string>
#include <map>

class AudioManager {
public:
    //phương thức để lấy intance duy nhất
    static AudioManager* Instance();
    //filePath: đường dẫn đến file âm thanh, id: tên định danh để truy xuất sau này
    bool loadSound(const std::string& filePath, const std::string& id);
    //filePath: đường dẫn đến file nhạc, id: tên đđịnh đanh dể sau này truy cập
    bool loadMusic(const std::string& filePath, const std::string& id);
    //id: tên hiệu ứng đã tải, loop: vòng lặp(= 0 phát 1 lần ,= -1 lặp vộ hạn)
    void playSound(const std::string& id, int loop = 0);
    void playMusic(const std::string& id, int loop = -1);
    //đặt âm lượng cho hiệu ứng
    void setSoundVolume(int volume);
    //đặt âm lượng cho âm nhạc
    void setMusicVolume(int volume);
private:
    //constructor private ngăn việc tạo instance từ bên ngoài
    AudioManager();
    //con trỏ instance duy nhất
    static AudioManager*s_pIntance;
    // Danh sách hiệu ứng âm thanh (SFX)
    // Key: string id, Value: Mix_Chunk* (SDL chứa dữ liệu âm thanh ngắn)
    std::map<std::string, Mix_Chunk*> m_sfxs;
     // Danh sách nhạc nền
    // Key: string id, Value: Mix_Music* (SDL chứa dữ liệu nhạc dài)
    std::map<std::string, Mix_Music*> m_music;
};

#endif // AUDIOMANAGER_H_INCLUDED
