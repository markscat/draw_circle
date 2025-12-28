#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Algorithm.h"
#include "PaintFrame.h"

#include <QElapsedTimer> // 必須包含這個標頭檔


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_algorithm(new Algorithm(200, 200, 50))
    , m_useParametric(false)
{
    ui->setupUi(this);

    // 【重點 1】將成員指標指向 UI 裡的物件
    m_paintFrame = ui->Paint_frame;
    m_paintFrame->setAlgorithm(m_algorithm);


    // 【重點 2】設定 Slider 範圍，否則預設可能是 0
    ui->Diameter_horizontalSlider->setRange(1, 200);
    ui->Diameter_horizontalSlider->setValue(50);

    // 【重點 3】初始化 ComboBox 選項
    ui->Algorithm_comboBox->addItem("Midpoint");
    ui->Algorithm_comboBox->addItem("Parametric");
    ui->Algorithm_comboBox->addItem("Bresenham"); // 新增選項


    // Timer 用於動畫
    m_timer = new QTimer(this);

    connect(m_timer, &QTimer::timeout, [=](){
        // 不管是哪種模式，通通呼叫 next()
        if (!m_algorithm->next()) {
            m_timer->stop();
        }
        m_paintFrame->update();
    });


    // 在 Timer 的 connect 區塊內
    connect(m_timer, &QTimer::timeout, [=](){
        if (!m_algorithm->next()) {
            m_timer->stop();
            // 當動畫停止時，計算總耗時
            double totalSeconds = m_animationTimer.elapsed() / 1000.0;
            ui->Time_label->setText(QString("動畫總計: %1 s").arg(totalSeconds));
        }
        m_paintFrame->update();
    });

/*
    connect(m_timer, &QTimer::timeout, [=](){
        bool running = false;
        if (ui->Algorithm_comboBox->currentIndex() == 0) running = m_algorithm->nextMidpoint();
        else if (ui->Algorithm_comboBox->currentIndex() == 1) running = m_algorithm->nextParametric();
        else running = m_algorithm->nextBresenham();

        if (!running) m_timer->stop();
        m_paintFrame->update();
    });
*/

    // 按鈕連線
    connect(ui->Start_pushButton_2, &QPushButton::clicked, this, &MainWindow::onStart);
    connect(ui->step_pushButton, &QPushButton::clicked, this, &MainWindow::onStep);
    connect(ui->Stop_pushButton, &QPushButton::clicked, this, &MainWindow::onStop);

    // 半徑 slider
    connect(ui->Diameter_horizontalSlider, &QSlider::valueChanged, this, &MainWindow::onDiameterChanged);

    // 下拉選單切換算法 (必須在 UI 中加一個 ComboBox)
     connect(ui->Algorithm_comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onAlgorithmChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::onStart()
{
    updateAlgorithmCenter(); // <--- 確保圓心在中心
    m_algorithm->reset();
    m_paintFrame->update();    // 清空畫面
    m_animationTimer.start(); // 開始計時

    m_timer->start(30); // 每 30ms 走一步
}

void MainWindow::updateAlgorithmCenter() {
    // 取得畫布目前的中心點
    int centerX = m_paintFrame->width() / 2;
    int centerY = m_paintFrame->height() / 2;

    // 更新演算法的圓心
    m_algorithm->setCenter(centerX, centerY);
}


void MainWindow::onStep()
{

    // 如果是第一次執行（點集是空的），更新圓心
    if (m_algorithm->getMidpointPoints().empty() &&
        m_algorithm->getParametricPoints().empty() &&
        m_algorithm->getBresenhamPoints().empty()) {
        updateAlgorithmCenter();
    }

    m_algorithm->next();
    /*
    if (m_useParametric)
        m_algorithm->nextParametric();
    else
        m_algorithm->nextMidpoint();
    */

    m_paintFrame->update();
}

void MainWindow::onStop()
{
    m_timer->stop();
}

void MainWindow::onDiameterChanged(int value)
{
    ui->Diameter_lineEdit->setText(QString::number(value)); // 順便更新輸入框
    m_algorithm->setRadius(value);

    // 重設算法狀態
    m_algorithm->resetMidpoint();
    m_algorithm->resetParametric();
    m_algorithm->resetBresenham();
    //新增演算法時,可以在這邊新增

    m_paintFrame->update();
}

void MainWindow::onAlgorithmChanged(int index)
{
    m_timer->stop();
    updateAlgorithmCenter(); // <--- 切換時也更新一次圓心


    // 根據索引設定模式
    CircleMode mode = static_cast<CircleMode>(index);

    // 1. 設定邏輯層的模式
    m_algorithm->setMode(mode);

    // 2. 設定繪圖層的模式，不然 PaintFrame 永遠只會畫 Midpoint 的 vector
    m_paintFrame->setMode(mode);


    // 重設對應的演算法狀態（或是全部重設也可以）
    switch (mode) {
    case CircleMode::Midpoint:
        m_algorithm->resetMidpoint();
        break;

    case CircleMode::Parametric:
        m_algorithm->resetParametric();
        break;

    case CircleMode::Bresenham:
        m_algorithm->resetBresenham();
        break;

        // 未來若增加新演算法，只需在此新增 case：
        // case CircleMode::NewAlgorithm:
        //     m_algorithm->resetNewAlgorithm();
        //     break;

    default:
        // 處理未預期的情況
        break;
    }
    // 更新繪圖框的模式 (假設 PaintFrame 也有對應的 setMode)

    m_paintFrame->update();
}
