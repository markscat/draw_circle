#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "Algorithm.h"
#include "PaintFrame.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Algorithm *m_algorithm;
    PaintFrame *m_paintFrame;
    QTimer *m_timer;
    bool m_useParametric;
    QElapsedTimer m_animationTimer;
    void updateAlgorithmCenter();

    QTimer *m_waveTimer; // 專屬波形的計時器


private slots:
    void onStart();
    void onStep();
    void onStop();
    void onDiameterChanged(int value);
    void onAlgorithmChanged(int index);
};
#endif // MAINWINDOW_H
