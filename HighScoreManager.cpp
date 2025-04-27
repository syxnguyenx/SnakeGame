#include "HighScoreManager.h"
#include <algorithm>
#include <SDL_mixer.h>
HighScoreManager* HighScoreManager::s_pInstance = nullptr;

HighScoreManager* HighScoreManager::Instance() {
    if(s_pInstance == nullptr) {
        s_pInstance = new HighScoreManager();
    }
    return s_pInstance;
}

HighScoreManager::HighScoreManager() {
    loadScores();
}

void HighScoreManager::loadScores() {
    std::ifstream file(m_filePath, std::ios::binary);
    if(file.is_open()) {
        int score;
        while(file.read(reinterpret_cast<char*>(&score), sizeof(int))) {
            m_scores.push_back(score);
        }
        file.close();
    }
}

void HighScoreManager::saveScores() {
    std::ofstream file(m_filePath, std::ios::binary);
    if(file.is_open()) {
        for(int score : m_scores) {
            file.write(reinterpret_cast<const char*>(&score), sizeof(int));
        }
        file.close();
    }
}

void HighScoreManager::addScore(int score) {
    m_scores.push_back(score);
    std::sort(m_scores.rbegin(), m_scores.rend());
    if(m_scores.size() > 10) {
        m_scores.resize(10);
    }
    saveScores();
}

int HighScoreManager::getHighScore() const {
    if(m_scores.empty()) return 0;
    return m_scores[0];
}
