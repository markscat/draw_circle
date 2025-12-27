#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Algorithm.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_algorithm(new Algorithm(200, 200, 50))
    , m_useParametric(false)
{
    ui->setupUi(this);

    m_paintFrame = new PaintFrame(this);
    m_paintFrame->setGeometry(ui->Paint_frame->geometry());
    m_paintFrame->setAlgorithm(m_algorithm);
    m_paintFrame->show();


    // Timer 用於動畫
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, [=](){
        bool running = m_useParametric ? m_algorithm->nextParametric()
                                       : m_algorithm->nextMidpoint();
        if (!running) m_timer->stop();
        m_paintFrame->update();
    });


    // 按鈕連線
    connect(ui->Start_pushButton_2, &QPushButton::clicked, this, &MainWindow::onStart);
    connect(ui->step_pushButton, &QPushButton::clicked, this, &MainWindow::onStep);
    connect(ui->Stop_pushButton, &QPushButton::clicked, this, &MainWindow::onStop);

    // 半徑 slider
    connect(ui->Diameter_horizontalSlider, &QSlider::valueChanged, this, &MainWindow::onDiameterChanged);

    // 下拉選單切換算法 (必須在 UI 中加一個 ComboBox)
    // connect(ui->algorithmComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
    //        this, &MainWindow::onAlgorithmChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onStep()
{
    if (m_useParametric)
        m_algorithm->nextParametric();
    else
        m_algorithm->nextMidpoint();

    m_paintFrame->update();
}

void MainWindow::onStop()
{
    m_timer->stop();
}

void MainWindow::onDiameterChanged(int value)
{
    m_algorithm->setRadius(value);
    m_algorithm->resetMidpoint();
    m_algorithm->resetParametric();
    m_paintFrame->update();
}

void MainWindow::onAlgorithmChanged(int index)
{
    // 0 = Midpoint, 1 = Parametric
    m_useParametric = (index == 1);
    if (m_useParametric)
        m_algorithm->resetParametric();
    else
        m_algorithm->resetMidpoint();

    m_paintFrame->setUseParametric(m_useParametric);
    m_paintFrame->update();
}
