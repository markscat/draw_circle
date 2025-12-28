#ifndef PAINTFRAME_H
#define PAINTFRAME_H
#pragma once

#include <QFrame>
#include <vector>
#include <utility>
#include "Algorithm.h"

class PaintFrame : public QFrame
{
    Q_OBJECT
public:
    explicit PaintFrame(QWidget *parent = nullptr);

    void setAlgorithm(Algorithm *algo) { m_algorithm = algo; }

    void setUseParametric(bool use) { m_useParametric = use; }
    void setMode(CircleMode mode) { m_currentMode = mode; }


protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Algorithm *m_algorithm = nullptr;
    bool m_useParametric = false;

    CircleMode m_currentMode = CircleMode::Midpoint; // 儲存目前的模式

};

#endif // PAINTWIDGET_H
