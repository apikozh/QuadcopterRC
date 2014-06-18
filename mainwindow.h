#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>

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

private slots:
    void onSocketRead();
    void on_btnTerminate_clicked();
    void on_btnArm_clicked();
    void on_btnDisarm_clicked();
    void on_btnCalibrateGyro_clicked();
    void on_btnCalibrateAccel_clicked();
    void on_btnSetPID_clicked();
    void on_btnSetLogCfg_clicked();
};

#endif // MAINWINDOW_H
