#ifndef ALGORITHM_H
#define ALGORITHM_H
#pragma once
#include <vector>
#include <utility>

class Algorithm {
public:
    Algorithm(int cx, int cy, int radius);

    // 設定半徑
    void setRadius(int r) { m_radius = r; }

    // Midpoint 圓演算法
    void resetMidpoint();
    bool nextMidpoint(); // 逐步生成下一組點
    const std::vector<std::pair<int,int>>& getMidpointPoints() const { return m_midPoints; }

    // Parametric 圓演算法
    void resetParametric();
    bool nextParametric(); // 逐步生成下一個點
    const std::vector<std::pair<int,int>>& getParametricPoints() const { return m_paramPoints; }

private:
    int m_cx, m_cy;
    int m_radius;

    // Midpoint 狀態
    int x_mid, y_mid;
    int d_mid;
    std::vector<std::pair<int,int>> m_midPoints;

    // Parametric 狀態
    double theta_param;
    double deltaTheta;
    std::vector<std::pair<int,int>> m_paramPoints;

    // Midpoint 助手
    void addSymmetricMidpointPoints(int x, int y);
};



#endif // ALGORITHM_H
