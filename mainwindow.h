#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include "tracelogstore.h"
#include "pidgraphdata.h"
#include "accelgraphdata.h"
#include "gyrographdata.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setControlParams(int throttle, int pitch, int roll, int yaw);
    
private:
    Ui::MainWindow *ui;
    QUdpSocket socket;
    QHostAddress copterAddr;
    quint16 copterPort;

    TraceLogStore traceLog;
    PIDGraphData* pidXGraphData;
    PIDGraphData* pidYGraphData;
    PIDGraphData* pidZGraphData;
    AccelGraphData* accelGraphData;
    GyroGraphData* gyroGraphData;
    AccelGraphData* accelRAWGraphData;
    GyroGraphData* gyroRAWGraphData;

private slots:
    void onSocketRead();
    void onTraceLogDataChanged();
    void on_btnTerminate_clicked();
    void on_btnArm_clicked();
    void on_btnDisarm_clicked();
    void on_btnCalibrateGyro_clicked();
    void on_btnCalibrateAccel_clicked();
    void on_btnSetPID_clicked();
    void on_btnSetLogCfg_clicked();
    void on_traceLogScroller_valueChanged(int value);
};

#endif // MAINWINDOW_H
