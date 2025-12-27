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

    const auto &pts = m_useParametric ? m_algorithm->getParametricPoints()
                                      : m_algorithm->getMidpointPoints();

    for (auto p : pts) {
        painter.drawPoint(p.first, p.second);
    }
}
