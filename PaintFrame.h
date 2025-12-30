#ifndef PAINTFRAME_H
#define PAINTFRAME_H
#pragma once

#include <QFrame>
#include <vector>
#include <utility>
#include "Algorithm.h"

enum class FrameRole {
    CircleAlgorithm, // 原有的圓形演算法模式
    WaveCircle,      // 波形投影圓
    SineWave,        // 正弦波形
    CosineWave       // 餘弦波形
};

class PaintFrame : public QFrame
{
    Q_OBJECT
public:

    explicit PaintFrame(QWidget *parent = nullptr);

    void setAlgorithm(Algorithm *algo) { m_algorithm = algo; }

    void setMode(CircleMode mode) { m_currentMode = mode; }

    void set3DMode(bool enabled) { m_is3D = enabled; }


    //void setUseParametric(bool use) { m_useParametric = use; }

    void setRole(FrameRole role) { m_role = role; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Algorithm *m_algorithm = nullptr;

    bool m_is3D = false; // 預設為 2D

    // 狀態變數
    CircleMode m_currentMode = CircleMode::Midpoint; // 儲存目前的模式
    FrameRole m_role = FrameRole::CircleAlgorithm; // 預設角色


    // --- 這裡將不同的繪圖邏輯「命名化」 ---
    void drawCircleAlgorithms(QPainter *painter); // 原本的 Circle_Tab 邏輯
    void drawWaveCircle(QPainter *painter);      // Wave 投影圓
    void drawSineWave(QPainter *painter);        // 正弦波
    void drawCosineWave(QPainter *painter);      // 餘弦波

};

#endif // PAINTWIDGET_H
