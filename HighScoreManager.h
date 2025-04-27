#ifndef HIGH_SCORE_MANAGER_H
#define HIGH_SCORE_MANAGER_H

#include <string>
#include <fstream>
#include <algorithm>
#include <vector>

class HighScoreManager {
public:
    static HighScoreManager* Instance();

    void loadScores();
    void saveScores();
    void addScore(int score);
    int getHighScore() const;

private:
    HighScoreManager();
    static HighScoreManager* s_pInstance;
    std::vector<int> m_scores;
    const std::string m_filePath = "highscores.dat";
};

#endif
