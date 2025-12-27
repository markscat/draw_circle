#include "PaintFrame.h"
#include <QPainter>

PaintFrame::PaintFrame(QWidget *parent)
    : QFrame(parent)
{
    setMinimumSize(400, 400);
    setStyleSheet("background-color: white; border: 1px solid black;");
}

void PaintFrame::paintEvent(QPaintEvent *)
{
    if (!m_algorithm) return;

    QPainter painter(this);
    painter.setPen(Qt::black);


    const std::vector<std::pair<int,int>>* pts;

    switch (m_currentMode) {
    case CircleMode::Midpoint:
        pts = &m_algorithm->getMidpointPoints();
        break;
    case CircleMode::Parametric:
        pts = &m_algorithm->getParametricPoints();
        break;
    case CircleMode::Bresenham:
        pts = &m_algorithm->getBresenhamPoints();
        break;
    default:
        return;
    }

    for (const auto &p : *pts) {
        painter.drawPoint(p.first, p.second);
    }
}
