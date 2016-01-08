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

struct framedata
{
    quint8 spec1;
    quint8 spec2;
    quint8 datasize;
    quint8 cmd;
    quint8 avl;
    qint16 high1;
    qint16 high2;
    quint8 cnt;
    quint8 chksum;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    double get_altu(double ps);
    double get_ps(int h);
private slots:
    void on_spinBox_valueChanged(int arg1);

    void on_lineEdit_textChanged(const QString &arg1);

    void open_serial();
    void close_serial();
    void about();
    void readData();
    void clkout();
private:
    Ui::MainWindow *ui;
    void showStatusMessage(const QString &message);
    static const double R = 287.05287;
    static const double gn = 9.80665;
    static const double Tb0= 288.15;
    static const double Pb0 =101.325;
    static const double PbN =127.773730122932510017142;
    static const double Tb1= 216.65;
    static const double Pb1 =22.632040095007798974347;
    static const double Pb2 = 5.47487742428104650521766;
    static const double B = -0.0065;
    QSerialPort *serial;
     SettingsDialog  *settings;
     QLabel *status;
     QTimer  *clk;
     struct timeval st,ed;
     bool received;
     qint32 cycle;
     qint32 cnt;
     QList <qint32> cycque;
     QList<QByteArray> rect_queue;
};

#endif // MAINWINDOW_H
