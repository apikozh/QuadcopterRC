#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "msg.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    socket(this)
{
    ui->setupUi(this);

    setWindowState(Qt::WindowMaximized); // WindowFullScreen
    copterAddr = QHostAddress("192.168.222.1");
    copterPort = 6000;

    //socket.bind()
    connect(&socket, SIGNAL(readyRead()), this, SLOT(onSocketRead()));

    ui->controlArea->setMain(this);
    ui->tabWidget->setCurrentIndex(0);

    /*ui->controlArea->setDebugData(
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0);*/

}

MainWindow::~MainWindow()
{
    ui->btnDisarm->click();
    ui->boxLogLevel->setCurrentIndex(0);
    ui->btnSetLogCfg->click();
    delete ui;
}

void MainWindow::setControlParams(int throttle, int pitch, int roll, int yaw)
{
    MsgControl ctrl;
    ctrl.type = MSG_CONTROL;
    ctrl.throttle = throttle;
    ctrl.pitch = pitch;
    ctrl.roll = roll;
    ctrl.yaw = yaw;

    int res = socket.writeDatagram((const char*)&ctrl, sizeof(MsgControl), copterAddr, copterPort);
    if (res >= 0)
        qDebug() << QString().sprintf(
            "Send: T: %d, P: %d, R: %d, Y: %d",
            throttle, ctrl.pitch, ctrl.roll, ctrl.yaw);
}

void MainWindow::onSocketRead()
{
    if (socket.readBufferSize() > 500) {
        ui->log->appendPlainText("Received data to large");
        return;
    }
    char buf[501];
    char* text;
    LogMsg4* msg = (LogMsg4*)buf;
    int len = socket.readDatagram(buf, 500);
    switch (((LogMsg*)buf)->logLevel) {
        case 0:
            text = &((LogMsgText*)buf)->text;
            text[len-1] = 0;
            ui->log->appendPlainText(QString("Recv: ") + QString(text));
            break;
        case 4:
        case 3:
        case 2:
            ui->controlArea->setDebugData(
                    msg->rcThrottle*10,
                    msg->rcPitch*10,
                    msg->rcRoll*10,
                    msg->rcYaw*10,
                    msg->pitch*10,
                    msg->roll*10,
                    msg->yaw*10,
                    msg->motorFL*10,
                    msg->motorFR*10,
                    msg->motorBL*10,
                    msg->motorBR*10,
                    msg->P,
                    msg->I,
                    msg->D
            );
            ui->groupBox->setTitle(QString::number((int)(msg->interval*1000000)));
            break;
        case 1:
            ui->controlArea->setDebugData(
                        msg->rcThrottle*10,
                        msg->rcPitch*10,
                        msg->rcRoll*10,
                        msg->rcYaw*10,
                        msg->pitch*10,
                        msg->roll*10,
                        msg->yaw*10
                );
            //qDebug() << "Receive log message";
            break;
        default:
            buf[len] = 0;
            ui->log->appendPlainText(QString("Recv: ") + QString(text));
    }
}

void MainWindow::on_btnTerminate_clicked()
{
    MsgAction msg;
    msg.type = MSG_ACTION;
    msg.action = TERMINATE;
    int res = socket.writeDatagram((const char*)&msg, sizeof(MsgAction), copterAddr, copterPort);
    if (res >= 0) {
        ui->log->appendPlainText("Send: TERMINATE");
    }else{
        ui->log->appendPlainText("Error sending control message");
    }
}

void MainWindow::on_btnArm_clicked()
{
    MsgAction msg;
    msg.type = MSG_ACTION;
    msg.action = ARM;
    int res = socket.writeDatagram((const char*)&msg, sizeof(MsgAction), copterAddr, copterPort);
    if (res >= 0) {
        ui->log->appendPlainText("Send: ARM");
    }else{
        ui->log->appendPlainText("Error sending control message");
    }
}

void MainWindow::on_btnDisarm_clicked()
{
    MsgAction msg;
    msg.type = MSG_ACTION;
    msg.action = DISARM;
    int res = socket.writeDatagram((const char*)&msg, sizeof(MsgAction), copterAddr, copterPort);
    if (res >= 0) {
        ui->log->appendPlainText("Send: DISARM");
    }else{
        ui->log->appendPlainText("Error sending control message");
    }
}

void MainWindow::on_btnCalibrateGyro_clicked()
{
    MsgAction msg;
    msg.type = MSG_ACTION;
    msg.action = CALIBRATE_GYRO;
    int res = socket.writeDatagram((const char*)&msg, sizeof(MsgAction), copterAddr, copterPort);
    if (res >= 0) {
        ui->log->appendPlainText("Send: CALIBRATE_GYRO");
    }else{
        ui->log->appendPlainText("Error sending control message");
    }
}

void MainWindow::on_btnCalibrateAccel_clicked()
{
    MsgAction msg;
    msg.type = MSG_ACTION;
    msg.action = CALIBRATE_ACCEL;
    int res = socket.writeDatagram((const char*)&msg, sizeof(MsgAction), copterAddr, copterPort);
    if (res >= 0) {
        ui->log->appendPlainText("Send: CALIBRATE_ACCEL");
    }else{
        ui->log->appendPlainText("Error sending control message");
    }
}

void MainWindow::on_btnSetPID_clicked()
{
    MsgPID msg;
    msg.type = MSG_PID;

    msg.pid[PID_PITCH].P = 0;
    msg.pid[PID_PITCH].I = 0;
    msg.pid[PID_PITCH].D = 0;

    msg.pid[PID_ROLL].P = 0;
    msg.pid[PID_ROLL].I = 0;
    msg.pid[PID_ROLL].D = 0;

    msg.pid[PID_YAW].P = ui->edtYawP->value() / 100.0;
    msg.pid[PID_YAW].I = ui->edtYawI->value() / 100.0;
    msg.pid[PID_YAW].D = ui->edtYawD->value() / 100.0;

    msg.pid[PID_LEVEL].P = ui->edtLevelP->value() / 100.0;
    msg.pid[PID_LEVEL].I = ui->edtLevelI->value() / 100.0;
    msg.pid[PID_LEVEL].D = ui->edtLevelD->value() / 100.0;

    msg.pid[PID_MAG].P = ui->edtMagP->value() / 100.0;
    msg.pid[PID_MAG].I = ui->edtMagI->value() / 100.0;
    msg.pid[PID_MAG].D = ui->edtMagD->value() / 100.0;

    int res = socket.writeDatagram((const char*)&msg, sizeof(MsgPID), copterAddr, copterPort);
    if (res >= 0) {
        ui->log->appendPlainText(QString().sprintf(
            "Send: PID YAW (%f, %f, %f)",
            msg.pid[PID_YAW].P, msg.pid[PID_YAW].I, msg.pid[PID_YAW].D));
        ui->log->appendPlainText(QString().sprintf(
            "Send: PID LEVEL (%f, %f, %f)",
            msg.pid[PID_LEVEL].P, msg.pid[PID_LEVEL].I, msg.pid[PID_LEVEL].D));
    }else{
        ui->log->appendPlainText("Error sending control message");
    }
}

void MainWindow::on_btnSetLogCfg_clicked()
{
    MsgDebug msg;
    msg.type = MSG_DEBUG;
    msg.logLevel = ui->boxLogLevel->currentIndex();
    msg.sendPeriod = ui->boxPeriod->value();
    int res = socket.writeDatagram((const char*)&msg, sizeof(MsgDebug), copterAddr, copterPort);
    if (res >= 0) {
        ui->log->appendPlainText(QString().sprintf("Send: DEBUG (%d, %d)", msg.logLevel, msg.sendPeriod));
    }else{
        ui->log->appendPlainText("Error sending control message");
    }
}
