#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include<sys/time.h>
#include <QMainWindow>
#include<QtSerialPort/QSerialPort>
#include <QtCore/QtGlobal>
#include <QtCore/QQueue>
#include <QTimer>
namespace Ui {
class MainWindow;
}

class  SettingsDialog;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    double get_altu(double ps);
    double get_ps(int h);
    quint8 frmSTA;
    quint8 frmSize;
    quint8 frmChksum;
    quint8 frmdata[11];
private slots:
    void on_spinBox_valueChanged(int arg1);
    void on_lineEdit_textChanged(const QString &arg1);
    void open_serial();
    void close_serial();
    void about();
    void readData();
    void on_lineEdit_high1_textChanged(const QString &arg1);
    void on_lineEdit_high2_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    void showStatusMessage(const QString &message);
    const double R = 287.05287;
    const double gn = 9.80665;
    const double Tb0= 288.15;
    const double Pb0 =101.325;
    const double PbN =127.773730122932510017142;
    const double Tb1= 216.65;
    const double Pb1 =22.632040095007798974347;
    const double Pb2 = 5.47487742428104650521766;
    const double B = -0.0065;
    QSerialPort *serial;
     SettingsDialog  *settings;
     QLabel *status;
     struct timeval st,ed;
     bool received;
     bool echodata;
     qint32 cycle;
};

#endif // MAINWINDOW_H
