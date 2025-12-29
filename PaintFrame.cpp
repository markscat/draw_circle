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

    // 設定抗鋸齒，讓圓形和波形更平滑（選用）
    painter.setRenderHint(QPainter::Antialiasing);

    // 【角色調度員】只負責判斷誰該畫
    switch (m_role) {
    case FrameRole::CircleAlgorithm:
        drawCircleAlgorithms(&painter);
        break;
    case FrameRole::WaveCircle:
        drawWaveCircle(&painter);
        break;
    case FrameRole::SineWave:
        drawSineWave(&painter);
        break;
    case FrameRole::CosineWave:
        drawCosineWave(&painter);
        break;
    }
}


void PaintFrame::drawCircleAlgorithms(QPainter *painter)
{
    painter->setPen(Qt::black);
    const std::vector<std::pair<int,int>>* pts = nullptr;

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


    // 加上安全檢查，確保 pts 不是空指標
    if (pts) {
        for (const auto &p : *pts) {
            painter->drawPoint(p.first, p.second);
        }
    }


    /*
    if (!m_algorithm) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //painter.setPen(Qt::black);


    //const std::vector<std::pair<int,int>>* pts;
    const std::vector<std::pair<int,int>>* pts = nullptr;

*/

}

void PaintFrame::drawWaveCircle(QPainter *painter)
{
    // 1. 取得畫布中心與半徑
    int cx = width() / 2;
    int cy = height() / 2;
    //int r = 80; // 你也可以讓這連動 slider 的 radius

    int baseR = 100; // 基礎大圓半徑
    // 2. 取得目前的角度 (從 Algorithm 取得)
    double theta = m_algorithm->getWaveTheta();
    int count = m_algorithm->getHarmonicCount();

    int lastX = cx;
    int lastY = cy;

    for (int i = 0; i < count; ++i) {
        int n = 2 * i + 1; // 奇數項
        int r = static_cast<int>(baseR * (1.0 / n)); // 振幅隨頻率反比減少

        // 畫目前這個層級的圓
        painter->setPen(QPen(Qt::lightGray, 1));
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(QPoint(lastX, lastY), r, r);

        // 計算目前這個圓周上的點
        int curX = lastX + static_cast<int>(r * cos(n * theta));
        int curY = lastY - static_cast<int>(r * sin(n * theta));

        // 畫連心線
        painter->setPen(QPen(Qt::blue, 1));
        painter->drawLine(lastX, lastY, curX, curY);

        // 更新下一個圓的中心點
        lastX = curX;
        lastY = curY;
    }


    // 最終點畫一個小紅球
    painter->setBrush(Qt::red);
    painter->drawEllipse(QPoint(lastX, lastY), 4, 4);

    // 畫投影引導線
    painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
    painter->drawLine(lastX, lastY, width(), lastY);
    painter->drawLine(lastX, lastY, lastX, height());


/*

    // 3. 計算目前的點座標 (極座標轉直角座標)
    // 注意：Qt 的 Y 軸是向下的，所以 sin 要加負號才能讓波形向上升
    int px = cx + static_cast<int>(r * cos(theta));
    int py = cy - static_cast<int>(r * sin(theta));

    // --- 開始繪製 ---

    // A. 畫背景圓 (淡灰色)
    painter->setPen(QPen(Qt::lightGray, 1, Qt::SolidLine));
    painter->drawEllipse(QPoint(cx, cy), r, r);

    // B. 畫旋轉半徑 (向量)
    painter->setPen(QPen(Qt::blue, 2));
    painter->drawLine(cx, cy, px, py);

    // C. 畫旋轉點
    painter->setBrush(Qt::red);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(QPoint(px, py), 5, 5); // 畫一個半徑 5 的小球

    // D. 畫投影引導線 (虛線)
    painter->setPen(QPen(Qt::black, 1, Qt::DashLine));

    // 水平引導線：從旋轉點往右拉到畫布邊緣 (給 Sine 波接應)
    painter->drawLine(px, py, width(), py);

    // 垂直引導線：從旋轉點往下延伸到畫布邊緣 (給 Cosine 波接應)
    painter->drawLine(px, py, px, height());
*/
}

void PaintFrame::drawSineWave(QPainter *painter)
{
    const std::deque<double>& history = m_algorithm->getWaveHistory();
    if (history.empty()) return;

    int w = width();
    int h = height();
    int cy = h / 2;
    int r = 100;       // 基礎半徑，需與 drawWaveCircle 一致
    int xSpacing = 2;

    painter->setPen(QPen(Qt::lightGray, 1, Qt::DashLine));
    painter->drawLine(0, cy, w, cy);

    QPolygonF wavePoints;
    for (size_t i = 0; i < history.size(); ++i) {
        double theta = history[i];

        // 【核心修改】：呼叫 Algorithm 的諧波總和函式
        double harmonicSumY = m_algorithm->getHarmonicY(theta);

        double px = i * xSpacing;
        double py = cy - harmonicSumY * r; // 這裡 y 座標由諧波決定

        wavePoints << QPointF(px, py);
        if (px > w) break;
    }

    painter->setPen(QPen(Qt::green, 2));
    painter->drawPolyline(wavePoints);

    // 畫投影接點與引導線
    if (!wavePoints.empty()) {
        painter->setBrush(Qt::red);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(wavePoints[0], 4, 4);

        painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
        painter->drawLine(0, wavePoints[0].y(), w, wavePoints[0].y());
    }
}
void PaintFrame::drawCosineWave(QPainter *painter)
{
    const std::deque<double>& history = m_algorithm->getWaveHistory();
    if (history.empty()) return;

    int w = width();
    int h = height();
    int cx = w / 2;
    int r = 100;
    int ySpacing = 2;

    painter->setPen(QPen(Qt::lightGray, 1, Qt::DashLine));
    painter->drawLine(cx, 0, cx, h);

    QPolygonF wavePoints;
    for (size_t i = 0; i < history.size(); ++i) {
        double theta = history[i];

        // 【核心修改】：呼叫 Algorithm 的諧波總和函式
        double harmonicSumX = m_algorithm->getHarmonicX(theta);

        double px = cx + harmonicSumX * r; // 這裡 x 座標由諧波決定
        double py = i * ySpacing;

        wavePoints << QPointF(px, py);
        if (py > h) break;
    }

    painter->setPen(QPen(Qt::darkCyan, 2));
    painter->drawPolyline(wavePoints);

    if (!wavePoints.empty()) {
        painter->setBrush(Qt::red);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(wavePoints[0], 4, 4);

        painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
        painter->drawLine(wavePoints[0].x(), 0, wavePoints[0].x(), h);
    }
}


#ifdef NO_harmonic
void PaintFrame::drawSineWave(QPainter *painter)
{
    // 1. 取得歷史角度紀錄
    const std::deque<double>& history = m_algorithm->getWaveHistory();
    if (history.empty()) return;

    int w = width();
    int h = height();
    int cy = h / 2;    // Y 軸基準線（中心）
    int r = 80;        // 振幅（需與圓形半徑一致）
    int xSpacing = 2;  // 每個歷史點之間的水平間隔（像素）

    // 2. 畫背景基準線 (淺灰色)
    painter->setPen(QPen(Qt::lightGray, 1, Qt::DashLine));
    painter->drawLine(0, cy, w, cy);

    // 3. 準備波形路徑
    QPolygonF wavePoints;
    for (size_t i = 0; i < history.size(); ++i) {
        double theta = history[i];

        // 計算座標
        // x: 索引越大（越舊的點）越往右邊排
        double px = i * xSpacing;

        // y: cy - sin(theta) * r (減號是因為 Qt Y 軸向下)
        double py = cy - sin(theta) * r;

        wavePoints << QPointF(px, py);

        // 如果波形已經畫出畫布右邊界，就停止計算
        if (px > w) break;
    }

    // 4. 畫出正弦波 (綠色)
    painter->setPen(QPen(Qt::green, 2));
    painter->drawPolyline(wavePoints);

    // 5. 畫一個小紅點在波形的起點（左側），表示目前正連接著圓形的投影
    if (!wavePoints.empty()) {
        painter->setBrush(Qt::red);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(wavePoints[0], 4, 4);

        // 畫一條水平虛線貫穿整個 Sine 畫布，接應左邊圓形過來的引導線
        painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
        painter->drawLine(0, wavePoints[0].y(), w, wavePoints[0].y());
    }
}

void PaintFrame::drawCosineWave(QPainter *painter)
{
    // 1. 取得歷史角度紀錄
    const std::deque<double>& history = m_algorithm->getWaveHistory();
    if (history.empty()) return;

    int w = width();
    int h = height();
    int cx = w / 2;    // X 軸基準線（中心）
    int r = 80;        // 振幅（需與圓形半徑一致）
    int ySpacing = 2;  // 每個歷史點之間的垂直間隔（像素）

    // 2. 畫背景基準線 (淺灰色垂直線)
    painter->setPen(QPen(Qt::lightGray, 1, Qt::DashLine));
    painter->drawLine(cx, 0, cx, h);

    // 3. 準備波形路徑
    QPolygonF wavePoints;
    for (size_t i = 0; i < history.size(); ++i) {
        double theta = history[i];

        // 計算座標
        // x: cx + cos(theta) * r
        double px = cx + cos(theta) * r;

        // y: i * ySpacing (索引越大越舊，越往下方排)
        double py = i * ySpacing;

        wavePoints << QPointF(px, py);

        // 如果波形已經畫出畫布下邊界，就停止計算
        if (py > h) break;
    }

    // 4. 畫出餘弦波 (深青色/藍綠色)
    painter->setPen(QPen(Qt::darkCyan, 2));
    painter->drawPolyline(wavePoints);

    // 5. 畫一個小紅點在波形的起點（上方），表示連接著圓形的投影
    if (!wavePoints.empty()) {
        painter->setBrush(Qt::red);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(wavePoints[0], 4, 4);

        // 畫一條垂直虛線貫穿整個 Cosine 畫布，接應上方圓形下來的引導線
        painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
        painter->drawLine(wavePoints[0].x(), 0, wavePoints[0].x(), h);
    }
}
#endif
