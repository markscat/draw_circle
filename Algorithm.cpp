#include "Algorithm.h"
#include <cmath>

Algorithm::Algorithm(int cx, int cy, int radius)
    : m_cx(cx), m_cy(cy), m_radius(radius), deltaTheta(0.01)
{
    resetMidpoint();
    resetParametric();
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
