#include "Algorithm.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Algorithm::Algorithm(int cx, int cy, int radius)
    : m_cx(cx), m_cy(cy), m_radius(radius), deltaTheta(0.01)
{
    m_currentMode = CircleMode::Midpoint; // 預設模式
    resetMidpoint();
    resetParametric();
    resetBresenham();

}


void Algorithm::setMode(CircleMode mode) {
    m_currentMode = mode;
}

// Algorithm.cpp
void Algorithm::reset() {
    switch (m_currentMode) {
    case CircleMode::Midpoint:
        resetMidpoint();
        break;
    case CircleMode::Parametric:
        resetParametric();
        break;
    case CircleMode::Bresenham:
        resetBresenham();
        break;
    }
}


// 統一的入口：未來增加演算法，MainWindow 都不用改，只要改這裡的 switch
bool Algorithm::next() {
    switch (m_currentMode) {
    case CircleMode::Midpoint:
        return nextMidpoint();
    case CircleMode::Parametric:
        return nextParametric();
    case CircleMode::Bresenham:
        return nextBresenham();
    default:
        return false;
    }
}

void Algorithm::setCenter(int cx, int cy) {
    m_cx = cx;
    m_cy = cy;
}

// ------------------- Midpoint -------------------
void Algorithm::resetMidpoint() {
    m_midPoints.clear();
    x_mid = 0;
    y_mid = m_radius;
    d_mid = 1 - m_radius;
}

bool Algorithm::nextMidpoint() {
    if (y_mid < x_mid) return false; // 完成
    addSymmetricMidpointPoints(x_mid, y_mid);

    if (d_mid < 0) {
        d_mid += 2*x_mid + 3;
    } else {
        d_mid += 2*(x_mid - y_mid) + 5;
        y_mid--;
    }
    x_mid++;
    return true;
}

void Algorithm::addSymmetricMidpointPoints(int x, int y) {
    m_midPoints.push_back({m_cx + x, m_cy + y});
    m_midPoints.push_back({m_cx - x, m_cy + y});
    m_midPoints.push_back({m_cx + x, m_cy - y});
    m_midPoints.push_back({m_cx - x, m_cy - y});
    m_midPoints.push_back({m_cx + y, m_cy + x});
    m_midPoints.push_back({m_cx - y, m_cy + x});
    m_midPoints.push_back({m_cx + y, m_cy - x});
    m_midPoints.push_back({m_cx - y, m_cy - x});
}

// ------------------- Parametric -------------------
void Algorithm::resetParametric() {
    m_paramPoints.clear();
    theta_param = 0;
}

bool Algorithm::nextParametric() {
    if (theta_param >= 2*M_PI) return false; // 完成
    int px = static_cast<int>(m_cx + m_radius * cos(theta_param));
    int py = static_cast<int>(m_cy + m_radius * sin(theta_param));
    m_paramPoints.push_back({px, py});
    theta_param += deltaTheta;
    return true;
}


void Algorithm::resetBresenham() {
    m_bresPoints.clear();
    x_bres = 0;
    y_bres = m_radius;
    d_bres = 3 - 2 * m_radius; // Bresenham 的初始判別式
}

bool Algorithm::nextBresenham() {
    if (y_bres < x_bres) return false;

    addEightPoints(x_bres, y_bres, m_bresPoints);

    if (d_bres < 0) {
        d_bres += 4 * x_bres + 6;
    } else {
        d_bres += 4 * (x_bres - y_bres) + 10;
        y_bres--;
    }
    x_bres++;
    return true;
}


// 這是專門給 Bresenham 用的對稱點函式，邏輯與 Midpoint 一樣，但存入 m_bresPoints
void Algorithm::addEightPoints(int x, int y, std::vector<std::pair<int,int>>& container) {
    m_bresPoints.push_back({m_cx + x, m_cy + y});
    m_bresPoints.push_back({m_cx - x, m_cy + y});
    m_bresPoints.push_back({m_cx + x, m_cy - y});
    m_bresPoints.push_back({m_cx - x, m_cy - y});
    m_bresPoints.push_back({m_cx + y, m_cy + x});
    m_bresPoints.push_back({m_cx - y, m_cy + x});
    m_bresPoints.push_back({m_cx + y, m_cy - x});
    m_bresPoints.push_back({m_cx - y, m_cy - x});
}



// ------------------- Wave Logic -------------------

void Algorithm::resetWave() {
    m_waveTheta = 0;
    m_waveHistory.clear();
}

/**
 * @brief 更新 Wave 的角度
 * @param deltaTheta 每次移動的角度步長（頻率）
 */
bool Algorithm::nextWave(double deltaTheta) {
    // 1. 增加角度
    m_waveTheta += deltaTheta;

    // 2. 保持角度在 0 ~ 2PI 之間（雖然不一定要，但比較整齊）
    if (m_waveTheta > 2 * M_PI) {
        m_waveTheta -= 2 * M_PI;
    }

    // 3. 將目前角度存入歷史紀錄的前端
    m_waveHistory.push_front(m_waveTheta);

    // 4. 如果超過最大長度，就把最舊的點丟掉，產生「流動感」
    if (m_waveHistory.size() > m_maxHistory) {
        m_waveHistory.pop_back();
    }

    return true; // 始終返回 true，因為波形通常是循環不斷的
}

double Algorithm::getWaveTheta() const {
    return m_waveTheta;
}

const std::deque<double>& Algorithm::getWaveHistory() const {
    return m_waveHistory;
}

double Algorithm::getHarmonicY(double theta) const {
    double sum = 0;
    // 只取奇數項：1, 3, 5, 7...
    for (int i = 0; i < m_harmonicCount; ++i) {
        int n = 2 * i + 1; // 奇數頻率
        sum += (1.0 / n) * sin(n * theta);
    }
    return sum; // 這裡不乘 4/pi 也可以，比例正確即可
}


#ifdef getHarmonicX_Type1
double Algorithm::getHarmonicX(double theta) const {
    double sum = 0;
    for (int i = 0; i < m_harmonicCount; ++i) {
        int n = 2 * i + 1;
        sum += (1.0 / n) * cos(n * theta);
    }
    return sum;
}
#endif

double Algorithm::getHarmonicX(double theta) const {
    double sum = 0;
    for (int i = 0; i < m_harmonicCount; ++i) {
        int n = 2 * i + 1;
        // 使用 (-1)^i 來讓係數正負交替：1, -1/3, 1/5, -1/7...
        double sign = (i % 2 == 0) ? 1.0 : -1.0;
        sum += (sign / n) * cos(n * theta);
    }
    return sum;
}
