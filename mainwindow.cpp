#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "msg.h"
#include "pidgraphdata.h"

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

    connect(&traceLog, SIGNAL(dataChanged()), this, SLOT(onTraceLogDataChanged()));
    //ui->traceLogScroller->setEnabled(false);

    traceLog.setMaxSize(10000);
    traceLog.setFile("sensorsLog.txt");

    ui->controlArea->setMain(this);
    ui->controlArea->setData(&traceLog);
    ui->tabWidget->setCurrentIndex(0);

    pidXGraphData = new PIDGraphData(&traceLog, PID_PITCH);
    pidYGraphData = new PIDGraphData(&traceLog, PID_ROLL);
    pidZGraphData = new PIDGraphData(&traceLog, PID_YAW);

    ui->pidXGraph->graph()->setData(pidXGraphData);
    ui->pidXGraph->setXScaleBounds(0.01, 10.0);
    ui->pidXGraph->setYScaleBounds(1.0, 100.0);
    ui->pidXGraph->setYScale(10.0);
    ui->pidXGraph->graph()->setIntervalScale(40);
    ui->pidXGraph->graph()->setVariableInterval(true);

    ui->pidYGraph->graph()->setData(pidYGraphData);
    ui->pidYGraph->setXScaleBounds(0.01, 10.0);
    ui->pidYGraph->setYScaleBounds(1.0, 100.0);
    ui->pidYGraph->setYScale(10.0);
    ui->pidYGraph->graph()->setIntervalScale(40);
    ui->pidYGraph->graph()->setVariableInterval(true);

    ui->pidZGraph->graph()->setData(pidZGraphData);
    ui->pidZGraph->setXScaleBounds(0.01, 10.0);
    ui->pidZGraph->setYScaleBounds(1.0, 100.0);
    ui->pidZGraph->setYScale(10.0);
    ui->pidZGraph->graph()->setIntervalScale(40);
    ui->pidZGraph->graph()->setVariableInterval(true);

    accelGraphData = new AccelGraphData(&traceLog);
    ui->accelGraph->graph()->setData(accelGraphData);
    ui->accelGraph->setXScaleBounds(0.01, 10.0);
    ui->accelGraph->setYScaleBounds(0.1, 10.0);
    ui->accelGraph->setYScale(5.0);
    ui->accelGraph->graph()->setIntervalScale(40);
    ui->accelGraph->graph()->setVariableInterval(true);

    gyroGraphData = new GyroGraphData(&traceLog);
    ui->gyroGraph->graph()->setData(gyroGraphData);
    ui->gyroGraph->setXScaleBounds(0.01, 10.0);
    ui->gyroGraph->setYScaleBounds(0.1, 100.0);
    ui->gyroGraph->setYScale(1.3);
    ui->gyroGraph->graph()->setIntervalScale(40);
    ui->gyroGraph->graph()->setVariableInterval(true);

    accelRAWGraphData = new AccelGraphData(&traceLog, false);
    ui->accelRAWGraph->graph()->setData(accelRAWGraphData);
    ui->accelRAWGraph->setXScaleBounds(0.01, 10.0);
    ui->accelRAWGraph->setYScaleBounds(0.1, 10.0);
    ui->accelRAWGraph->setYScale(5.0);
    ui->accelRAWGraph->graph()->setIntervalScale(40);
    ui->accelRAWGraph->graph()->setVariableInterval(true);

    gyroRAWGraphData = new GyroGraphData(&traceLog, false);
    ui->gyroRAWGraph->graph()->setData(gyroRAWGraphData);
    ui->gyroRAWGraph->setXScaleBounds(0.01, 10.0);
    ui->gyroRAWGraph->setYScaleBounds(0.1, 100.0);
    ui->gyroRAWGraph->setYScale(1.3);
    ui->gyroRAWGraph->graph()->setIntervalScale(40);
    ui->gyroRAWGraph->graph()->setVariableInterval(true);

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

    /*LogMsg4 log;
    log.logLevel = 2;
    log.fullInterval = rand()*1.0/RAND_MAX/1000.0;
    log.P[0] = pitch/5.0;
    log.I[0] = roll/5.0;
    log.D[0] = yaw/5.0;
    log.pitch = pitch/10.0;
    log.roll = roll/10.0;
    log.yaw = yaw/10.0;
    log.rcPitch = pitch/10.0;
    log.rcRoll = roll/10.0;
    log.rcYaw = yaw/10.0;
    log.rcThrottle = throttle/10.0;
    log.motorFL = throttle/10.0 + pitch/100.0 + roll/100.0;
    log.motorFR = throttle/10.0 + pitch/100.0 - roll/100.0;
    log.motorBL = throttle/10.0 - pitch/100.0 + roll/100.0;
    log.motorBR = throttle/10.0 - pitch/100.0 - roll/100.0;
    traceLog.addMessage(&log);*/
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
    switch (msg->logLevel) {
        case 0:
            text = &((LogMsgText*)buf)->text;
            text[len-2] = 0;
            ui->log->appendPlainText(QString("Recv: ") + QString(text));
            break;
        case 4: case 3: case 2: case 1:
            traceLog.setFrozen(!msg->armed);
            traceLog.addMessage(msg);

            if (msg->armed && ui->btnArm->styleSheet().isEmpty()) {
                ui->btnArm->setStyleSheet("QPushButton {\nbackground-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 rgba(170, 255, 0, 255), stop:1 rgba(0, 255, 0, 255));}");
            }else if (!msg->armed && !ui->btnArm->styleSheet().isEmpty()){
                ui->btnArm->setStyleSheet("");
            }

            ui->groupBox->setTitle(QString::number((int)(msg->lastInterval*1000000)) +
                                   "\t" +
                                   QString::number((int)(msg->fullInterval*1000000)));

            /*
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
            ui->groupBox->setTitle(QString::number((int)(msg->lastInterval*1000000)));

            ui->controlArea->setDebugData(
                        msg->rcThrottle*10,
                        msg->rcPitch*10,
                        msg->rcRoll*10,
                        msg->rcYaw*10,
                        msg->pitch*10,
                        msg->roll*10,
                        msg->yaw*10
                );
            if (msg->armed && ui->btnArm->styleSheet().isEmpty()) {
                ui->btnArm->setStyleSheet("QPushButton {\nbackground-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 rgba(170, 255, 0, 255), stop:1 rgba(0, 255, 0, 255));}");
            }else if (!msg->armed && !ui->btnArm->styleSheet().isEmpty()){
                ui->btnArm->setStyleSheet("");
            }*/
            //qDebug() << "Receive log message";
            break;
        default:
            buf[len-1] = 0;
            ui->log->appendPlainText(QString("Recv: ") + QString(buf));
    }
}

void MainWindow::onTraceLogDataChanged()
{
    ui->traceLogScroller->setMaximum(traceLog.getFullSize()-1);
    ui->traceLogScroller->setValue(
                ui->traceLogScroller->maximum() - traceLog.getOffset());
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

void MainWindow::on_traceLogScroller_valueChanged(int value)
{
    traceLog.setOffset(ui->traceLogScroller->maximum() - value);
}
