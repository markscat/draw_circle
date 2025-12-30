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

    if (!m_is3D) {
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
        return;
    }


    // --- 以下是 3D 模式邏輯 ---
    const std::deque<double>& history = m_algorithm->getWaveHistory();
    if (history.empty()) return;

    QPolygonF spiralPoints;

    // 我們遍歷歷史紀錄，將每一刻的「合成點」算出來並加上 Z 位移
    for (size_t i = 0; i < history.size(); ++i) {
        double theta = history[i];

        // 取得該時刻的合成 X 和 Y (已包含所有諧波)
        double hx = m_algorithm->getHarmonicX(theta) * baseR;
        double hy = m_algorithm->getHarmonicY(theta) * baseR;

        // 虛擬 Z 軸：索引越老，Z 越深
        double hz = i * 1.5; // 控制螺旋拉伸的長度

        // 投影公式：將 (x, y, z) 轉換為螢幕 (x', y')
        // 我們讓 Z 軸往右上方 45 度角延伸
        double screenX = cx + hx + hz * 0.7;
        double screenY = cy - hy - hz * 0.4;

        spiralPoints << QPointF(screenX, screenY);

        // 畫出最前端那一組圓的「斷面」即可，不然圓太多會太亂
        if (i == 0) {
            // 這裡可以簡單畫一下目前的圓心結構
            painter->setPen(QPen(Qt::lightGray, 1));
            painter->drawPolyline(spiralPoints); // 暫時佔位
        }
    }

    // 畫出整條 3D 螺旋線
    painter->setPen(QPen(Qt::blue, 2));
    painter->drawPolyline(spiralPoints);

    // 在最前端（最新的點）畫個紅球
    if (!spiralPoints.empty()) {
        painter->setBrush(Qt::red);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(spiralPoints[0], 5, 5);

        // 3D 模式下的引導線：從紅球拉到畫布邊緣
        painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
        painter->drawLine(spiralPoints[0].x(), spiralPoints[0].y(), width(), spiralPoints[0].y());
        painter->drawLine(spiralPoints[0].x(), spiralPoints[0].y(), spiralPoints[0].x(), height());
    }

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
        //double sy = m_algorithm->getHarmonicY(theta) * r; // Sine 值 (上下)

        // 取得該時刻的諧波合成值
        double hy = m_algorithm->getHarmonicY(theta) * r; // Sine 分量 (高度)
        double hx = m_algorithm->getHarmonicX(theta) * r; // Cosine 分量 (深度)
        double screenX, screenY;

        //double px, py;
        if (m_is3D) {

            // --- 3D 投影模式 ---
            // i * xSpacing 是時間（向右延伸）
            // hx * 0.4 是將 X 軸的擺動投影為「深度偏移」
            screenX = (i * xSpacing) + (hx * 0.4);

            // cy - hy 是原始高度
            // hx * 0.2 是深度的垂直偏移（產生立體傾斜感）
            screenY = cy - hy - (hx * 0.2);
        } else {
            // --- 2D 平面模式 ---
            screenX = i * xSpacing;
            screenY = cy - hy;
        }

        // 【修正點】：要把算好的點放進點集裡，不然 wavePoints 永遠是空的
        wavePoints << QPointF(screenX, screenY);

        // 如果超出畫布且非 3D 模式則停止（3D 模式下因為有偏移，建議多畫一點）
        if (screenX > w && !m_is3D) break;
    }


    // 4. 繪製波形路徑
    painter->setRenderHint(QPainter::Antialiasing); // 開啟抗鋸齒
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
    // 1. 基礎數據準備
    const std::deque<double>& history = m_algorithm->getWaveHistory();
    if (history.empty()) return;

    int w = width();
    int h = height();
    int cx = w / 2;    // 垂直中心線 (Cosine 的基準)
    int r = 100;       // 振幅
    int ySpacing = 2;  // 時間軸（縱向）間距

    // 2. 畫背景基準線 (淺灰色垂直線)
    painter->setPen(QPen(Qt::lightGray, 1, Qt::DashLine));
    painter->drawLine(cx, 0, cx, h);

    // 3. 計算並收集點位
    QPolygonF wavePoints;
    for (size_t i = 0; i < history.size(); ++i) {
        double theta = history[i];

        // 取得該時刻的諧波合成值
        double hx = m_algorithm->getHarmonicX(theta) * r; // Cosine 分量 (主要的橫向擺動)
        double hy = m_algorithm->getHarmonicY(theta) * r; // Sine 分量 (3D 模式下的深度)

        double screenX, screenY;

        if (m_is3D) {
            // --- 3D 投影模式 ---
            // i * ySpacing 是時間（向下延伸）
            // hy * 0.4 作為橫向的深度偏移量，讓波形產生「左右扭動」的立體感
            screenX = (cx + hx) + (hy * 0.4);

            // i * ySpacing 是原始縱向位置
            // hy * 0.2 是深度的垂直修正
            screenY = (i * ySpacing) - (hy * 0.2);
        } else {
            // --- 2D 平面模式 ---
            screenX = cx + hx;
            screenY = i * ySpacing;
        }

        // 將點加入點集
        wavePoints << QPointF(screenX, screenY);

        // 如果超出畫布下邊界則停止
        if (screenY > h && !m_is3D) break;
    }

    // 4. 繪製波形路徑
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(Qt::darkCyan, 2));
    painter->drawPolyline(wavePoints);

    // 5. 繪製前端標記與垂直引導線
    if (!wavePoints.empty()) {
        // 繪製頂端的紅色接點
        painter->setBrush(Qt::red);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(wavePoints[0], 4, 4);

        // 畫一條貫穿畫布的垂直虛線，接應上方圓形傳來的 X 軸位置
        // 注意：在 3D 下，這條線會隨著投影點的 X 座標移動
        painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
        painter->drawLine(wavePoints[0].x(), 0, wavePoints[0].x(), h);
    }
}

/*
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
*/

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
