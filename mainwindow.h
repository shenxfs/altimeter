/**
 * @brief 主窗口类头文件
 *
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include<sys/time.h>
#include <QMainWindow>
#include<QtSerialPort/QSerialPort>
#include <QtCore/QtGlobal>
#include <QtCore/QQueue>
#include <QTimer>
namespace Ui
{
    class MainWindow;
}

class  SettingsDialog;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    double get_altu(double ps);
    double get_ps(int h);
    quint8 frmSTA;/**<接收数据包的状态机的状态标志*/
    quint8 frmSize;/**<接收包的长度*/
    quint8 frmChksum;/**<接收包的检验和*/
    quint8 frmdata[11];/**<接收包的缓冲区*/
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
    const double R = 287.05287;/**<气体常数*/
    const double gn = 9.80665;/**<重力加速度*/
    const double Tb0 = 288.15;/**<大气层下界（0～11000米）静温，开尔文*/
    const double Pb0 = 101.325;/**<大气层下界（0～11000米）静压，kPa*/
    const double PbN = 127.773730122932510017142;/**<大气层下界（-2000～0米）静压，kPa*/
    const double Tb1 = 216.65;/**<大气层下界（11000～20000米）静温，开尔文*/
    const double Pb1 = 22.632040095007798974347;/**<大气层下界（11000～20000米）静压，kPa*/
    const double Pb2 = 5.47487742428104650521766;/**<大气层下界（20000～32000米）静压，kPa*/
    const double Ps25km = 2.5110168179486166776514;/**<25000米对应的静压，kPa*/
    const double B = -0.0065;/**<大气层（0～11000米）温度梯度，K/m*/
    const double B1 = 0.001;/**<大气层（20000～32000米）温度梯度，K/m*/
    QSerialPort *serial;
    SettingsDialog  *settings;
    QLabel *status;
    struct timeval st, ed;
    bool received;
    bool echodata;
    qint32 cycle;
};

#endif // MAINWINDOW_H
