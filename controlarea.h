#ifndef CONTROLAREA_H
#define CONTROLAREA_H

#include "mainwindow.h"

#include <QWidget>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QTimer>

namespace Ui {
class ControlArea;
}

class ControlArea : public QWidget
{
    Q_OBJECT
    
public:
    explicit ControlArea(QWidget *parent = 0);
    ~ControlArea();

    void setMain(MainWindow* main);
    void setDebugData(int fbThrottle, int fbPitch,
        int fbRoll, int fbYaw, int rPitch, int rRoll, int rYaw,
        int motorFL = -1, int motorFR = -1, int motorBL = -1, int motorBR = -1,
        double P[] = 0, double I[] = 0, double D[] = 0);

private:
    Ui::ControlArea *ui;
    MainWindow* main;
    int throttle, pitch, roll, yaw;
    int fbThrottle, fbPitch, fbRoll, fbYaw;
    int rPitch, rRoll, rYaw;
    int motorFL, motorFR, motorBL, motorBR;
    double P[3], I[3], D[3];
    bool pidSet;
    QTimer timer;
    bool dataChanged;

    void wheelEvent(QWheelEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

private slots:
    void sendRCData();

};

#endif // CONTROLAREA_H
