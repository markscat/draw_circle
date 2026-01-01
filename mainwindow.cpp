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

    // 建立一個佈局
    QVBoxLayout *mainLayout = new QVBoxLayout(ui->centralwidget);
    // 將 TabWidget 加入佈局
    mainLayout->addWidget(ui->Circle_tabWidget);

    // 設定這行後，視窗縮放時 TabWidget 就會跟著動
    ui->centralwidget->setLayout(mainLayout);


    // --- 0. 先建立計時器實體 (確保指標安全) ---
    m_timer = new QTimer(this);
    m_waveTimer = new QTimer(this);

    // --- 5. 互斥處理：切換標籤頁時停止所有動畫 ---
    connect(ui->Circle_tabWidget, &QTabWidget::currentChanged, [=](int index){
        m_timer->stop();      // 停止圓形演算法計時器
        m_waveTimer->stop();  // 停止波形計時器
    });


    //Circle_Tab begin

    // 【重點 1】將成員指標指向 UI 裡的物件
    m_paintFrame = ui->Paint_frame;
    m_paintFrame->setAlgorithm(m_algorithm);


    // 【重點 2】設定 Slider 範圍，否則預設可能是 0
    ui->Diameter_horizontalSlider->setRange(5, 200);
    // 2. 設定初始數值 (例如預設 30ms)
    int initialSpeed = 30;
    ui->draw_Speed_Slider->setValue(initialSpeed);
    // 3. 立即更新 Label 的文字，讓它一開始就顯示 "30 ms"
    ui->draw_Speed_label->setText(QString("%1 ms").arg(initialSpeed));
    //ui->Diameter_horizontalSlider->setValue(50);

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
            double totalSeconds = m_animationTimer.elapsed() / 1000.0;
            ui->Time_label->setText(QString("動畫總計: %1 s").arg(totalSeconds));
        }
        m_paintFrame->update();
    });


    // 按鈕連線
    connect(ui->Start_pushButton_2, &QPushButton::clicked, this, &MainWindow::onStart);
    connect(ui->step_pushButton, &QPushButton::clicked, this, &MainWindow::onStep);
    connect(ui->Stop_pushButton, &QPushButton::clicked, this, &MainWindow::onStop);

    // 半徑 slider
    connect(ui->Diameter_horizontalSlider, &QSlider::valueChanged, this, &MainWindow::onDiameterChanged);

    // 下拉選單切換算法 (必須在 UI 中加一個 ComboBox)
     connect(ui->Algorithm_comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onAlgorithmChanged);

   //Circle_Tab end


    // Wave_Tab begin

     // --- 1. 初始化 Wave_tab 的畫框角色 ---
     ui->Circle_frame->setAlgorithm(m_algorithm);
     ui->Circle_frame->setRole(FrameRole::WaveCircle);

     ui->Sin_frame->setAlgorithm(m_algorithm);
     ui->Sin_frame->setRole(FrameRole::SineWave);

     ui->cos_frame->setAlgorithm(m_algorithm);
     ui->cos_frame->setRole(FrameRole::CosineWave);

     // --- 2. 初始化頻率選單 ---
     // 使用 QComboBox 的 userData 儲存每次增加的角度步長 (deltaTheta)
     ui->Frequency_comboBox->addItem("極慢 (0.02)", 0.02);
     ui->Frequency_comboBox->addItem("慢 (0.05)", 0.05);
     ui->Frequency_comboBox->addItem("一般 (0.1)", 0.1);
     ui->Frequency_comboBox->addItem("快 (0.2)", 0.2);
     ui->Frequency_comboBox->setCurrentIndex(2); // 預設一般


     // --- 3. 設定 Wave 動畫計時器 ---
     m_waveTimer = new QTimer(this);

     connect(m_waveTimer, &QTimer::timeout, [=](){
         // 從 ComboBox 取得目前的頻率數值
         double speed = ui->Frequency_comboBox->currentData().toDouble();

         // 更新演算法的角度歷史
         m_algorithm->nextWave(speed);

         // 同步更新三個畫框
         ui->Circle_frame->update();
         ui->Sin_frame->update();
         ui->cos_frame->update();
     });


     // --- 4. 按鈕連線 ---
     connect(ui->Wave_Start_pushButton, &QPushButton::clicked, [=](){
         m_waveTimer->start(30);
     });

     connect(ui->Wave_Stop_pushButton, &QPushButton::clicked, [=](){
         m_waveTimer->stop();
     });

     connect(ui->step_pushButton, &QPushButton::clicked, [=](){
         double speed = ui->Frequency_comboBox->currentData().toDouble();
         m_algorithm->nextWave(speed);
         ui->Circle_frame->update();
         ui->Sin_frame->update();
         ui->cos_frame->update();
     });

     ui->Harmonic_comboBox->addItem("1 Harmonic (Pure)", 1);
     ui->Harmonic_comboBox->addItem("2 Harmonics", 2);
     ui->Harmonic_comboBox->addItem("3 Harmonics", 3);
     ui->Harmonic_comboBox->addItem("5 Harmonics", 5);
     ui->Harmonic_comboBox->addItem("10 Harmonics (Square)", 10);
     ui->Harmonic_comboBox->addItem("20 Harmonics (Sharp)", 20);

     connect(ui->Harmonic_comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
         int count = ui->Harmonic_comboBox->currentData().toInt();
         m_algorithm->setHarmonicCount(count);
         // 更新畫面讓使用者即時看到圓的變化
         ui->Circle_frame->update();
         ui->Sin_frame->update();
         ui->cos_frame->update();
     });



     connect(ui->towDtheD_radioButton, &QRadioButton::toggled, [=](bool checked){

         ui->Circle_frame->set3DMode(checked);
         ui->Sin_frame->set3DMode(checked);
         ui->cos_frame->set3DMode(checked);

         ui->Circle_frame->update();
         ui->Sin_frame->update();
         ui->cos_frame->update();

     });

     connect(ui->draw_Speed_Slider, &QSlider::valueChanged, [=](int value){
         // 如果計時器正在跑，就即時更新它的速度
         if (m_timer->isActive()) m_timer->start(value);
         if (m_waveTimer->isActive()) m_waveTimer->start(value);

         // 可以在 Label 顯示目前是幾 ms 一步
         ui->draw_Speed_label->setText(QString("%1 ms").arg(value));
     });
     // Wave_Tab end

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

    // 修改這裡：從 Slider 取得目前設定的速度，而不是固定 50
    int currentSpeed = ui->draw_Speed_Slider->value();
    m_timer->start(currentSpeed);

    //m_timer->start(50); // 每 30ms 走一步
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
