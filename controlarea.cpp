#include "controlarea.h"
#include "ui_controlarea.h"

#include "mainwindow.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <QPainter>
#include <QLineF>
#include <QRectF>

int constrain(int val, int min, int max) {
    if (val < min)
        val = min;
    if (val > max)
        val = max;
    return val;
}

ControlArea::ControlArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlArea),
    timer(this)
{
    ui->setupUi(this);
    throttle = pitch = roll = yaw = 0;
    fbThrottle = fbPitch = fbRoll = fbYaw = 0;
    rPitch = rRoll = rYaw = 0;
    motorFL = motorFR = motorBL = motorBR = -1;
    for (int i=0; i<3; i++) {
        P[i] = 0;
        I[i] = 0;
        D[i] = 0;
    }
    pidSet = false;

    dataChanged = false;

    connect(&timer, SIGNAL(timeout()), this, SLOT(sendRCData()));
    timer.start(50);
}

ControlArea::~ControlArea()
{
    delete ui;
}

void ControlArea::sendRCData()
{
    if (dataChanged) {
        main->setControlParams(throttle, pitch, roll, yaw);
        dataChanged = false;
    }
}


void ControlArea::setMain(MainWindow* main) {
    this->main = main;
}

void ControlArea::setDebugData(int fbThrottle, int fbPitch,
    int fbRoll, int fbYaw, int rPitch, int rRoll, int rYaw,
    int motorFL, int motorFR, int motorBL, int motorBR,
    double P[], double I[], double D[])
{
    this->fbThrottle = fbThrottle;
    this->fbPitch = fbPitch;
    this->fbRoll = fbRoll;
    this->fbYaw = fbYaw;
    this->rPitch = rPitch;
    this->rRoll = rRoll;
    this->rYaw = rYaw;
    this->motorFL = motorFL;
    this->motorFR = motorFR;
    this->motorBL = motorBL;
    this->motorBR = motorBR;
    if (P && I && D) {
        for (int i=0; i<3; i++) {
            this->P[i] = P[i];
            this->I[i] = I[i];
            this->D[i] = D[i];
        }
        pidSet = true;
    }else
        pidSet = false;
    update();
}


void ControlArea::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QRectF rect;
    int xB, xE, yB, yE, size;
    if (width() < height()) {
        xB = 0;
        xE = width() - 1;
        yB = height()/2 - width()/2;
        yE = height()/2 + width()/2 - 1;
        size = width();
    }else{
        yB = 0;
        yE = height() - 1;
        xB = width()/2 - height()/2;
        xE = width()/2 + height()/2 - 1;
        size = height();
    }

    float w, h;
    int tw, th;

    // Draw frame
    rect.setRect(xB, yB, size-1, size-1);
    painter.drawRect(rect);
    painter.drawLine(xB, height()/2, xE, height()/2);
    painter.drawLine(width()/2, yB, width()/2, yE);

    // Real Pitch, Roll and Yaw
    float radius = 3.0*size/14.0;
    float mid = size/4.0;
    QString text;

    // Pitch
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QColor(Qt::black));
    painter.drawEllipse(
                width()/2.0 - mid - radius,
                height()/2.0 - mid - radius,
                radius*2, radius*2);

    painter.setPen(QColor(160, 160, 160));
    painter.setFont(QFont("MS Shell Dlg 2", size/20));
    text = "Pitch";
    w = painter.fontMetrics().width(text);
    h = painter.fontMetrics().height();
    painter.drawText(width()/2.0 - mid - w/2.0,
                     height()/2.0 - mid - radius/2.0 + h/2.0, text);
    painter.setPen(QColor(0, 160, 160));
    painter.setFont(QFont("MS Shell Dlg 2", size/30));
    text = QString::number(qRound(rPitch/10.0));
    w = painter.fontMetrics().width(text);
    h = painter.fontMetrics().height();
    text += "°";
    painter.drawText(width()/2.0 - mid - w/2.0,
                     height()/2.0 - mid + radius/3.0 + h/2.0, text);

    painter.setPen(QPen(QColor(Qt::black), 3));
    xB = width()/2.0 - mid + radius*cos(-rPitch*M_PI/1800);
    xE = width()/2.0 - mid - radius*cos(-rPitch*M_PI/1800);
    yB = height()/2.0 - mid + radius*sin(-rPitch*M_PI/1800);
    yE = height()/2.0 - mid - radius*sin(-rPitch*M_PI/1800);
    painter.drawLine(xB, yB, xE, yE);
    xB = width()/2.0 - mid - radius*5/7*cos(-rPitch*M_PI/1800);
    yB = height()/2.0 - mid - radius*5/7*sin(-rPitch*M_PI/1800);
    xE = width()/2.0 - mid + radius*6/7*cos((-rPitch-1600)*M_PI/1800);
    yE = height()/2.0 - mid + radius*6/7*sin((-rPitch-1600)*M_PI/1800);
    painter.drawLine(xB, yB, xE, yE);
    painter.setBrush(QBrush(QColor(Qt::black), Qt::SolidPattern));
    painter.setPen(QPen(QColor(Qt::black), 1));
    painter.drawEllipse(
                width()/2.0 - mid - radius/30.0,
                height()/2.0 - mid - radius/30.0,
                radius*2/30.0, radius*2/30.0);

    // Roll
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QColor(Qt::black));
    painter.drawEllipse(
                width()/2.0 + mid - radius,
                height()/2.0 - mid - radius,
                radius*2, radius*2);

    painter.setPen(QColor(160, 160, 160));
    painter.setFont(QFont("MS Shell Dlg 2", size/20));
    text = "Roll";
    w = painter.fontMetrics().width(text);
    h = painter.fontMetrics().height();
    painter.drawText(width()/2.0 + mid - w/2.0,
                     height()/2.0 - mid - radius/2.0 + h/2.0, text);
    painter.setPen(QColor(0, 160, 160));
    painter.setFont(QFont("MS Shell Dlg 2", size/30));
    text = QString::number(qRound(rRoll/10.0));
    w = painter.fontMetrics().width(text);
    h = painter.fontMetrics().height();
    text += "°";
    painter.drawText(width()/2.0 + mid - w/2.0,
                     height()/2.0 - mid + radius/3.0 + h/2.0, text);

    painter.setPen(QPen(QColor(Qt::black), 3));
    xB = width()/2.0 + mid + radius*cos(rRoll*M_PI/1800);
    xE = width()/2.0 + mid - radius*cos(rRoll*M_PI/1800);
    yB = height()/2.0 - mid + radius*sin(rRoll*M_PI/1800);
    yE = height()/2.0 - mid - radius*sin(rRoll*M_PI/1800);
    painter.drawLine(xB, yB, xE, yE);
    xB = width()/2.0 + mid;
    yB = height()/2.0 - mid;
    xE = width()/2.0 + mid + radius/4*cos((rRoll-900)*M_PI/1800);
    yE = height()/2.0 - mid + radius/4*sin((rRoll-900)*M_PI/1800);
    painter.drawLine(xB, yB, xE, yE);
    painter.setBrush(QBrush(QColor(Qt::black), Qt::SolidPattern));
    painter.setPen(QPen(QColor(Qt::black), 1));
    painter.drawEllipse(
                width()/2.0 + mid - radius/30.0,
                height()/2.0 - mid - radius/30.0,
                radius*2/30.0, radius*2/30.0);

    // Yaw
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QColor(Qt::black));
    painter.drawEllipse(
                width()/2.0 - mid - radius,
                height()/2.0 + mid - radius,
                radius*2, radius*2);

    painter.setPen(QColor(160, 160, 160));
    painter.setFont(QFont("MS Shell Dlg 2", size/20));
    text = "Yaw";
    w = painter.fontMetrics().width(text);
    h = painter.fontMetrics().height();
    painter.drawText(width()/2.0 - mid - w/2.0,
                     height()/2.0 + mid - radius/2.0 + h/2.0, text);
    painter.setPen(QColor(0, 160, 160));
    painter.setFont(QFont("MS Shell Dlg 2", size/30));
    text = QString::number(qRound(rYaw/10.0));
    w = painter.fontMetrics().width(text);
    h = painter.fontMetrics().height();
    text += "°";
    painter.drawText(width()/2.0 - mid - w/2.0,
                     height()/2.0 + mid + radius/3.0 + h/2.0, text);

    painter.setPen(QPen(QColor(Qt::black), 3));
    xB = width()/2.0 - mid + radius*cos(-(rYaw+900)*M_PI/1800);
    xE = width()/2.0 - mid - radius*cos(-(rYaw+900)*M_PI/1800);
    yB = height()/2.0 + mid + radius*sin(-(rYaw+900)*M_PI/1800);
    yE = height()/2.0 + mid - radius*sin(-(rYaw+900)*M_PI/1800);
    painter.drawLine(xB, yB, xE, yE);
    xE = width()/2.0 - mid + radius*6/7*cos((-(rYaw+900)-100)*M_PI/1800);
    yE = height()/2.0 + mid + radius*6/7*sin((-(rYaw+900)-100)*M_PI/1800);
    painter.drawLine(xB, yB, xE, yE);
    xE = width()/2.0 - mid + radius*6/7*cos((-(rYaw+900)+100)*M_PI/1800);
    yE = height()/2.0 + mid + radius*6/7*sin((-(rYaw+900)+100)*M_PI/1800);
    painter.drawLine(xB, yB, xE, yE);
    painter.setBrush(QBrush(QColor(Qt::black), Qt::SolidPattern));
    painter.setPen(QPen(QColor(Qt::black), 1));
    painter.drawEllipse(
                width()/2.0 - mid - radius/30.0,
                height()/2.0 + mid - radius/30.0,
                radius*2/30.0, radius*2/30.0);


    // Thtottle & Motors
    QColor powerFillColor(100, 255, 100);
    QColor powerTextColor(Qt::black);

    w = mid/4.0;

    painter.setBrush(Qt::NoBrush);
    painter.setPen(QColor(Qt::black));
    xB = width()/2.0 + size/2.0 - w;
    yB = height()/2.0;
    rect.setRect(xB, yB, w, size/2.0);
    painter.drawRect(rect);

    // Feedback Thtottle
    painter.setBrush(QBrush(powerFillColor, Qt::SolidPattern));
    painter.setPen(powerFillColor);
    xB++;
    yB += (1000.0-fbThrottle)*(size/2.0-2)/1000.0 + 1;
    rect.setRect(xB, yB, w-2, fbThrottle*(size/2.0-2)/1000.0);
    painter.drawRect(rect);

    // Sent Thtottle
    painter.setPen(QPen(QColor(Qt::black), 2));
    yB = height()/2.0 + (1000.0-throttle)*(size/2.0-2)/1000.0 + 1;
    painter.drawLine(xB, yB, xB+w-2, yB);

    // Percent
    painter.setPen(powerTextColor);
    painter.setFont(QFont("MS Shell Dlg 2", size/50));
    text = QString::number(qRound(throttle/10.0)) + "%";
    tw = painter.fontMetrics().width(text);
    th = painter.fontMetrics().height();
    painter.drawText(xB + w/2.0 - 1 - tw/2.0,
                     height()/2.0 + mid, text);

    painter.setPen(powerTextColor);
    painter.setFont(QFont("MS Shell Dlg 2", size/50));
    text = QString::number(qRound(fbThrottle/10.0)) + "%";
    tw = painter.fontMetrics().width(text);
    th = painter.fontMetrics().height();
    painter.drawText(xB + w/2.0 - 1 - tw/2.0,
                     height()/2.0 + mid + th, text);

    // Motors
    if (motorFL >= 0 && motorFR >= 0 && motorBL >= 0 && motorBR >= 0) {
        h = mid*4/5.0;
        w = mid/4.0;
        radius = mid/2.0;

        // Front Left
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QColor(Qt::black));
        xB = width()/2.0 + mid - radius - w/2.0;
        yB = height()/2.0 + mid - radius - h/2.0;
        rect.setRect(xB, yB, w, h);
        painter.drawRect(rect);

        painter.setBrush(QBrush(powerFillColor, Qt::SolidPattern));
        painter.setPen(powerFillColor);
        xB++;
        yB += (1000 - motorFL)*(h-2)/1000 + 1;
        rect.setRect(xB, yB, w-2, motorFL*(h-2)/1000);
        painter.drawRect(rect);

        painter.setPen(powerTextColor);
        painter.setFont(QFont("MS Shell Dlg 2", size/50));
        text = QString::number(qRound(motorFL/10.0)) + "%";
        tw = painter.fontMetrics().width(text);
        th = painter.fontMetrics().height();
        painter.drawText(width()/2.0 + mid - radius - tw/2.0,
                         height()/2.0 + mid - radius + th/2.0, text);

        // Front Right
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QColor(Qt::black));
        xB = width()/2.0 + mid + radius - w/2.0;
        yB = height()/2.0 + mid - radius - h/2.0;
        rect.setRect(xB, yB, w, h);
        painter.drawRect(rect);

        painter.setBrush(QBrush(powerFillColor, Qt::SolidPattern));
        painter.setPen(powerFillColor);
        xB++;
        yB += (1000 - motorFR)*(h-2)/1000 + 1;
        rect.setRect(xB, yB, w-2, motorFR*(h-2)/1000);
        painter.drawRect(rect);

        painter.setPen(powerTextColor);
        painter.setFont(QFont("MS Shell Dlg 2", size/50));
        text = QString::number(qRound(motorFR/10.0)) + "%";
        tw = painter.fontMetrics().width(text);
        th = painter.fontMetrics().height();
        painter.drawText(width()/2.0 + mid + radius - tw/2.0,
                         height()/2.0 + mid - radius + th/2.0, text);

        // Back Left
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QColor(Qt::black));
        xB = width()/2.0 + mid - radius - w/2.0;
        yB = height()/2.0 + mid + radius - h/2.0;
        rect.setRect(xB, yB, w, h);
        painter.drawRect(rect);

        painter.setBrush(QBrush(powerFillColor, Qt::SolidPattern));
        painter.setPen(powerFillColor);
        xB++;
        yB += (1000 - motorBL)*(h-2)/1000 + 1;
        rect.setRect(xB, yB, w-2, motorBL*(h-2)/1000);
        painter.drawRect(rect);

        painter.setPen(powerTextColor);
        painter.setFont(QFont("MS Shell Dlg 2", size/50));
        text = QString::number(qRound(motorBL/10.0)) + "%";
        tw = painter.fontMetrics().width(text);
        th = painter.fontMetrics().height();
        painter.drawText(width()/2.0 + mid - radius - tw/2.0,
                         height()/2.0 + mid + radius + th/2.0, text);

        // Back Right
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QColor(Qt::black));
        xB = width()/2.0 + mid + radius - w/2.0;
        yB = height()/2.0 + mid + radius - h/2.0;
        rect.setRect(xB, yB, w, h);
        painter.drawRect(rect);

        painter.setBrush(QBrush(powerFillColor, Qt::SolidPattern));
        painter.setPen(powerFillColor);
        xB++;
        yB += (1000 - motorBR)*(h-2)/1000 + 1;
        rect.setRect(xB, yB, w-2, motorBR*(h-2)/1000);
        painter.drawRect(rect);

        painter.setPen(powerTextColor);
        painter.setFont(QFont("MS Shell Dlg 2", size/50));
        text = QString::number(qRound(motorBR/10.0)) + "%";
        tw = painter.fontMetrics().width(text);
        th = painter.fontMetrics().height();
        painter.drawText(width()/2.0 + mid + radius - tw/2.0,
                         height()/2.0 + mid + radius + th/2.0, text);
    }

    // PID
    if (pidSet) {
        // PID X
        painter.setPen(QColor(Qt::black));
        painter.setFont(QFont("MS Shell Dlg 2", size/50));
        text = QString::number(P[0], 'f', 2) + "%";
        tw = painter.fontMetrics().width(text);
        th = painter.fontMetrics().height();
        painter.drawText(width()/2.0 + mid - mid/2.0 - tw/2.0,
                         height()/2.0 + mid + th/2.0, text);

        painter.setPen(QColor(Qt::black));
        painter.setFont(QFont("MS Shell Dlg 2", size/50));
        text = QString::number(I[0], 'f', 2) + "%";
        tw = painter.fontMetrics().width(text);
        th = painter.fontMetrics().height();
        painter.drawText(width()/2.0 + mid - tw/2.0,
                         height()/2.0 + mid + th/2.0, text);

        painter.setPen(QColor(Qt::black));
        painter.setFont(QFont("MS Shell Dlg 2", size/50));
        text = QString::number(D[0], 'f', 2) + "%";
        tw = painter.fontMetrics().width(text);
        th = painter.fontMetrics().height();
        painter.drawText(width()/2.0 + mid + mid/2.0 - tw/2.0,
                         height()/2.0 + mid + th/2.0, text);

        // PID Y
        painter.setPen(QColor(Qt::black));
        painter.setFont(QFont("MS Shell Dlg 2", size/50));
        text = QString::number(P[1], 'f', 2) + "%";
        tw = painter.fontMetrics().width(text);
        th = painter.fontMetrics().height();
        painter.drawText(width()/2.0 + mid - tw/2.0,
                         height()/2.0 + mid - mid*5/6.0 + th/2.0, text);

        painter.setPen(QColor(Qt::black));
        painter.setFont(QFont("MS Shell Dlg 2", size/50));
        text = QString::number(I[1], 'f', 2) + "%";
        tw = painter.fontMetrics().width(text);
        th = painter.fontMetrics().height();
        painter.drawText(width()/2.0 + mid - tw/2.0,
                         height()/2.0 + mid - mid*4/6.0 + th/2.0, text);

        painter.setPen(QColor(Qt::black));
        painter.setFont(QFont("MS Shell Dlg 2", size/50));
        text = QString::number(D[1], 'f', 2) + "%";
        tw = painter.fontMetrics().width(text);
        th = painter.fontMetrics().height();
        painter.drawText(width()/2.0 + mid - tw/2.0,
                         height()/2.0 + mid - mid*3/6.0 + th/2.0, text);

        // PID Z
        painter.setPen(QColor(Qt::black));
        painter.setFont(QFont("MS Shell Dlg 2", size/50));
        text = QString::number(P[2], 'f', 2) + "%";
        tw = painter.fontMetrics().width(text);
        th = painter.fontMetrics().height();
        painter.drawText(width()/2.0 + mid + mid/6.0 - tw/2.0,
                         height()/2.0 + mid + mid*3/6.0 + th/2.0, text);

        painter.setPen(QColor(Qt::black));
        painter.setFont(QFont("MS Shell Dlg 2", size/50));
        text = QString::number(I[2], 'f', 2) + "%";
        tw = painter.fontMetrics().width(text);
        th = painter.fontMetrics().height();
        painter.drawText(width()/2.0 + mid - mid/6.0 - tw/2.0,
                         height()/2.0 + mid + mid*4/6.0 + th/2.0, text);

        painter.setPen(QColor(Qt::black));
        painter.setFont(QFont("MS Shell Dlg 2", size/50));
        text = QString::number(D[2], 'f', 2) + "%";
        tw = painter.fontMetrics().width(text);
        th = painter.fontMetrics().height();
        painter.drawText(width()/2.0 + mid + mid/6.0 - tw/2.0,
                         height()/2.0 + mid + mid*5/6.0 + th/2.0, text);
    }



    // Feedback Yaw
    painter.setBrush(QBrush(QColor(100, 100, 255), Qt::SolidPattern));
    painter.setPen(QColor(100, 100, 255));
    rect.setRect(width()/2 - fbYaw * size /600 - 2, height()/2 - 14,
                 4, 28);
    painter.drawRect(rect);

    // Sent Yaw
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(Qt::black), 2));
    painter.drawLine(width()/2 - yaw * size /600, height()/2 - 12,
                     width()/2 - yaw * size /600, height()/2 + 12);

    // Feedback Pitch & Roll
    painter.setBrush(QBrush(QColor(100, 100, 255), Qt::SolidPattern));
    painter.setPen(QColor(100, 100, 255));
    painter.drawEllipse(width()/2 + fbRoll * size /600 - 5,
                        height()/2 + fbPitch * size /600 - 5, 10, 10);

    // Sent Pitch & Roll
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(Qt::black), 2));
    painter.drawEllipse(width()/2 + roll * size /600 - 6,
                        height()/2 + pitch * size /600 - 6, 12, 12);
}

void ControlArea::wheelEvent(QWheelEvent* event)
{
    if (event->delta()) {
        throttle += event->delta() / 10;
        throttle = constrain(throttle, 0, 1000);
        dataChanged = true;
    }
    //main->setControlParams(throttle, pitch, roll, yaw);
    update();
}

void ControlArea::mouseMoveEvent(QMouseEvent* event) {
    int min = width() < height() ? width() : height();
    int x = event->x();
    int y = event->y();

    if (event->buttons().testFlag(Qt::LeftButton)) {
        pitch = (y - height()/2) * 600.0 / min;
        roll = (x - width()/2) * 600.0 / min;
        yaw = 0;
    }else if (event->buttons().testFlag(Qt::RightButton)) {
        pitch = 0;
        roll = 0;
        yaw = (width()/2 - x) * 600.0 / min;
    }

    pitch = constrain(pitch, -300, 300);
    roll = constrain(roll, -300, 300);
    yaw = constrain(yaw, -300, 300);

    dataChanged = true;

    //main->setControlParams(throttle, pitch, roll, yaw);
    update();
}

void ControlArea::mousePressEvent(QMouseEvent* event) {
    mouseMoveEvent(event);
}

void ControlArea::mouseReleaseEvent(QMouseEvent* event) {
    pitch = roll = yaw = 0;
    dataChanged = true;
    //main->setControlParams(throttle, pitch, roll, yaw);
    update();
}
