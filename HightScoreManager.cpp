#include "HightScoreManager.h"
#include <algorithm>

HightScoreManager* HightScoreManager::s_pInstance = nullptr;
HightScoreManager* HightScoreManager::Instance() {
    if(s_pInstance == nullptr) {
        s_pInstance = new HightScoreManager();
    }
    return s_pInstance;
}
HightScoreManager* HightScoreManager() {
    loadScore();
}

void HightScoreManager::loadScore() {
    std::ifstream file(m_filePath, std::ios::binary);
    if(file.is_open()) {
        int score;
        while(file.read(reinterpret_cast<char*>(&score), sizeof(int))) {
            m_scores.push_back(score);
        }
        file.close();
    }
}

void HightScoreManager::saveSocre() {
    std::ofstream file(m_filePath, std::ios::binary);
    if(file.is_open()) {
        for(int score : m_scores) {
            file.write(reinterpret_cast<const char*>(&score), sizeof(int));
        }
        file.close();
    }
}

void HightScoreManager::addScore(int score) {
    m_scores.push_back(score);
    std::sort(m_scores.rbegin(), m_scores.rend());
    if(m_scores.size() > 10) {
        m_scores.resize(10);
    }
    saveSocre();
}

int HightScoreManager::getHightScore() const {
    if(m_scores.empty()) return 0;
    return m_scores[0];
}
