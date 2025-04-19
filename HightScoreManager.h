#ifndef HIGHTSCOREMANAGER_H_INCLUDED
#define HIGHTSCOREMANAGER_H_INCLUDED

#include <string>
#include <fstream>
#include <algorithm>

class HightScoreManager {
public:
    //phương thức static để lấy intance duy nhất
    static HightScoreManager* Instance();
    //tải điểm vào file
    void loadScore();
    //lưu điểm vào file
    void saveSocre();
    //thêm đêirm mới và tự động sắp xếp, score: điẻm cần thêm
    void addScore(int score);
    //trả về điểm cao nhất
    int getHightScore() const;
private:
    //constructor private để nagwns việc tạo instance từ bên ngoài
    HightScoreManager();
    //con trỏ duy nhất
    static HightScoreManager* s_pIntance;
    //danh sách điểm số
    std::vector<int> m_scores;
    //đường dẫn đến file lưu điểm
    const std::string m_filePath = "hightscores.dat";
};

#endif // HIGHTSCOREMANAGER_H_INCLUDED
