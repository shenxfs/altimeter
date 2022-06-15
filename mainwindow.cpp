/**
 * @brief 主窗口
 * @file mainwindow.cpp
 * @author shenxf (380406785@qq.com)
 * @version V2.0.0
 * @date 2022-06-15
 *
 * 主窗口类实现 高度与静压相互转换，高度范围-2000～25000米，通过串口接收数据并解析，检测数据包的间隔周期并显示，显示数据包的标志字节。
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"settingsdialog.h"
#include <qmath.h>

#include <QMessageBox>
#include <QLabel>
#include <QDebug>

/**
 * @brief MainWindow::MainWindow
 * @param parent
 *
 * 主窗口类构造函数
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    settings = new SettingsDialog;
    serial = new QSerialPort(this);
    status = new QLabel;
    ui->statusBar->addWidget(status);
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionSetup->setEnabled(true);
    ui->actionAbout->setEnabled(true);
    ui->highEdit->setText(tr("0"));
    ui->lineEdit->setText(tr("101.325"));
    connect(ui->actionSetup, SIGNAL(triggered()), settings, SLOT(show()));
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(open_serial()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(close_serial()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
    received = false;
    echodata = false;
    cycle = 0;
    st.tv_sec = 0;
    st.tv_usec = 0;
    ed.tv_sec = 0;
    ed.tv_usec = 0;
    frmSTA = 0U;
    frmSize = 0U;
    frmChksum = 0U;
    showStatusMessage(tr("未连接"));
}

/**
 * @brief MainWindow::~MainWindow
 *
 * 析构函数
 */
MainWindow::~MainWindow()
{
    delete ui;
    delete settings;
    delete status;
}

/**
 * @brief MainWindow::on_spinBox_valueChanged
 * @param arg1 高度值
 *
 * 高度选择框内容改变回调函数，显示对应的静压值
 */
void MainWindow::on_spinBox_valueChanged(int arg1)
{
    QString str;
    QTextStream s(&str);
    s.setFieldWidth(7);
    s.setFieldAlignment(QTextStream::AlignRight);
    s.setRealNumberPrecision(3);
    s << Qt::fixed << get_ps(arg1);
    ui->lineEdit->setText(str);
}

/**
 * @brief MainWindow::on_lineEdit_textChanged
 * @param arg1 静压值
 *
 * 静压编辑框内容改变回调函数，显示对应高度值
 */
void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    QString str;
    QTextStream s(&str);
    s.setFieldWidth(6);
    s.setFieldAlignment(QTextStream::AlignRight);
    s.setRealNumberPrecision(0);
    s << Qt::fixed << get_altu(arg1.toDouble());
    ui->highEdit->setText(str);
}

/**
 * @brief MainWindow::get_altu
 * @param ps 静压值
 * @return 高度值
 *
 * 静压转高度
 */
double MainWindow::get_altu(double ps)
{
    double high;
    if(ps > PbN)
    {
        high = -2000;
    }
    else if(ps > Pb1)
    {
        high = Tb0 / B * (qExp(-B * R / gn * qLn(ps / Pb0)) - 1.0);
    }
    else if (ps > Pb2)
    {
        high = 11000  + (- R * Tb1 / gn * qLn(ps / Pb1) );
    }
    else if(ps > Ps25km)
    {
        high = 20000 + Tb1 / B1 * (qExp(-B1 * R / gn * qLn(ps / Pb2)) - 1.0);
    }
    else
    {
        high = 25000;
    }
    return high;
}

/**
 * @brief MainWindow::get_ps
 * @param h 高度值
 * @return 静压值
 *
 * 高度转静压
 */
double MainWindow::get_ps(int h)
{
    double ps;
    if(h < -2000)
    {
        ps = PbN;
    }
    else if(h < 11000)
    {
        ps = Pb0 * qPow(((h * B) + Tb0) / Tb0, -gn / R / B);
    }
    else if(h < 20000)
    {
        ps = Pb1 * qExp(-gn / Tb1 / R * (h - 11000));
    }
    else if(h < 25000)
    {
        ps = Pb2 * qPow((((h - 20000) * B1) + Tb1) / Tb1, -gn / R / B1);
    }
    else
    {
        ps  = Ps25km;
    }
    return ps;
}

/**
 * @brief MainWindow::open_serial
 *
 * 打开按钮槽函数，打开串口
 */
void MainWindow::open_serial()
{
    SettingsDialog::Settings p = settings->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadOnly))
    {
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->actionSetup->setEnabled(false);
        showStatusMessage(tr("已连接至 %1 : %2, %3, %4, %5, %6")
                          .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    }
    else
    {
        QMessageBox::critical(this, tr("错误"), serial->errorString());
        showStatusMessage(tr("串口打开错误"));
    }
    echodata = true;
    ui->textEdit->setText("");
}

/**
 * @brief MainWindow::close_serial
 *
 * 关闭按钮槽函数，关闭串口
 */
void MainWindow::close_serial()
{
    if (serial->isOpen())
    {
        serial->close();
    }
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionSetup->setEnabled(true);
    showStatusMessage(tr("已断开"));
    received = false;
    cycle = 0;
    echodata = false;
}

/**
 * @brief MainWindow::about
 *
 * 显示版本信息
 */
void MainWindow::about()
{
    QMessageBox::about(this, tr("关于高度传感器显示程序V2.0.0"),
                       tr("高度传感器显示程序基于Qt5开发，将海拔高度(m)转换为压力(kPa)并显示，"
                          "通过接收高度传感器串行信号，显示高度传感器2路海拔高度结果，并与给定的海拔高度进行"
                          "比较作出是否符合要求的结论，检测高度传感器串行信号的更新率和完整性。"));
}

void MainWindow::showStatusMessage(const QString &message)
{
    status->setText(message);
}

/**
 * @brief MainWindow::readData
 *
 * 串口接收回调函数 处理数据包信息
 */
void MainWindow::readData()
{
    QByteArray data = serial->readAll();
    quint8 i, ch;
    for(i = 0; i < data.size(); i++)
    {
        ch = static_cast<quint8>(data.at(i));
        if(0 == frmSTA)
        {
            if(0xAAU == ch)
            {
                frmdata[0] = ch;
                frmChksum += ch;
                if(!received)
                {
                    gettimeofday(&st, nullptr);
                    received = true;
                }
                else
                {
                    gettimeofday(&ed, nullptr);
                    cycle = static_cast<qint32>(ed.tv_sec - st.tv_sec) * 1000000L + (ed.tv_usec - st.tv_usec);
                    st.tv_sec = ed.tv_sec;
                    st.tv_usec = ed.tv_usec;
                }
                frmSTA = 1U;
            }
            else
            {
                frmSTA = 0;
                frmChksum = 0;
            }
        }
        else if(1U == frmSTA)
        {
            if(0x55U == ch)
            {
                frmdata[1] = ch;
                frmChksum += ch;
                frmSTA = 2U;
            }
            else
            {
                frmSTA = 0;
                frmChksum = 0;
            }
        }
        else if(2U == frmSTA)
        {
            if(0x0BU == ch)
            {
                frmdata[2] = ch;
                frmChksum += ch;
                frmSTA = 3U;
            }
            else
            {
                frmSTA = 0;
                frmChksum = 0;
            }
        }
        else if(3U == frmSTA)
        {
            if(0xC0U == ch)
            {
                frmdata[3] = ch;
                frmChksum += ch;
                frmSTA = 4U;
            }
            else
            {
                frmSTA = 0;
                frmChksum = 0;
            }
        }
        else if(4U == frmSTA)
        {
            frmdata[frmSize + 4] = ch;
            frmChksum += ch;
            frmSize++;
            if(frmSize >= 6U)
            {
                frmSTA = 5U;
                frmSize = 0;
            }
        }
        else if(5U == frmSTA)
        {
            frmChksum += ch;
            if(0 == frmChksum)
            {
                frmdata[10] = ch;
                QString strOne = "";
                QString str;
                for(quint8 j = 0; j < 11; j++)
                {
                    str = QString("%1 ").arg(static_cast<quint8>(frmdata[j]), 2, 16, QLatin1Char('0'));
                    strOne += str;
                }
                ui->textEdit->append(strOne);
                quint16 high = 0;
                high = static_cast<quint16>(frmdata[6]) * 256U + static_cast<quint8>(frmdata[5]);
                str = QString("%1").arg(high, 5, 10);
                ui->lineEdit_high1->setText(str);
                high = static_cast<quint16>(frmdata[8]) * 256U + static_cast<quint8>(frmdata[7]);
                str = QString("%1").arg(high, 5, 10);
                ui->lineEdit_high2->setText(str);
                if(echodata)
                {
                    str = QString("%1").arg(static_cast<quint8>(frmdata[4]), 2, 16, QLatin1Char('0'));
                    ui->lineEdit_4->setText(str);
                    str = QString("%1").arg(cycle / 1000.0, 5, 'g', 3);
                    ui->lineEdit_5->setText(str);
                }
            }
            frmSTA = 0;
            frmChksum = 0;
        }
        else
        {
            frmSTA = 0;
            frmChksum = 0;
        }
    }
}

/**
 * @brief MainWindow::on_lineEdit_high1_textChanged
 * @param arg1 高度值
 *
 * 高度1编辑框内容改变回调函数，显示对应静压
 */
void MainWindow::on_lineEdit_high1_textChanged(const QString &arg1)
{
    QString str;
    QTextStream s(&str);
    s.setFieldWidth(7);
    s.setFieldAlignment(QTextStream::AlignRight);
    s.setRealNumberPrecision(3);
    s << Qt::fixed << get_ps(arg1.toInt());
    ui->lineEdit_2->setText(str);
}

/**
 * @brief MainWindow::on_lineEdit_high2_textChanged
 * @param arg1 高度值
 *
 * 高度2编辑框内容改变回调函数，显示对应静压
 */
void MainWindow::on_lineEdit_high2_textChanged(const QString &arg1)
{
    QString str;
    QTextStream s(&str);
    s.setFieldWidth(7);
    s.setFieldAlignment(QTextStream::AlignRight);
    s.setRealNumberPrecision(3);
    s << Qt::fixed << get_ps(arg1.toInt());
    ui->lineEdit_3->setText(str);
}
