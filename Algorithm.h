#ifndef ALGORITHM_H
#define ALGORITHM_H
#pragma once
#include <vector>
#include <utility>

enum class CircleMode { Midpoint, Parametric, Bresenham };


class Algorithm {
public:
    Algorithm(int cx, int cy, int radius);

    // 設定半徑
    void setRadius(int r) { m_radius = r; }

    // 新增：設定目前的模式與統一的執行介面
    void setMode(CircleMode mode);
    void reset(); // <--- 新增這一行
    bool next();

    void setCenter(int cx, int cy);

    // Midpoint 圓演算法
    void resetMidpoint();
    bool nextMidpoint(); // 逐步生成下一組點
    const std::vector<std::pair<int,int>>& getMidpointPoints() const { return m_midPoints; }

    // Parametric 圓演算法
    void resetParametric();
    bool nextParametric(); // 逐步生成下一個點
    const std::vector<std::pair<int,int>>& getParametricPoints() const { return m_paramPoints; }


    // --- Bresenham (新增) ---
    void resetBresenham();
    bool nextBresenham();
    const std::vector<std::pair<int,int>>& getBresenhamPoints() const { return m_bresPoints; }


private:
    int m_cx, m_cy;
    int m_radius;

    CircleMode m_currentMode; // 儲存當前模式


    // Midpoint 狀態
    int x_mid, y_mid;
    int d_mid;
    std::vector<std::pair<int,int>> m_midPoints;

    // Parametric 狀態
    double theta_param;
    double deltaTheta;
    std::vector<std::pair<int,int>> m_paramPoints;


    int x_bres, y_bres, d_bres; // Bresenham 狀態
    std::vector<std::pair<int,int>> m_bresPoints;

    // Midpoint 助手
    void addSymmetricMidpointPoints(int x, int y);

    // 通用對稱點助手 (修改為可傳入目標 vector)
    void addEightPoints(int x, int y, std::vector<std::pair<int,int>>& container);
};



#endif // ALGORITHM_H
