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

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Algorithm *m_algorithm = nullptr;
    bool m_useParametric = false;
};

#endif // PAINTWIDGET_H
