#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"settingsdialog.h"
#include <qmath.h>

#include <QMessageBox>
#include <QLabel>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    settings = new SettingsDialog;
    serial = new QSerialPort(this);
    status = new QLabel;
    clk = new QTimer;
    clk->start(1000);
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
    connect(clk, SIGNAL(timeout()), this, SLOT(clkout()));
    received = false;
    cycle =0;
    cnt  =0;
    st.tv_sec = 0;
    st.tv_usec = 0;
    ed.tv_sec = 0;
    ed.tv_usec = 0;
    qDebug()<<"size of struct framedata= "<<sizeof(struct framedata);
}
MainWindow::~MainWindow()
{
    delete ui;
    delete settings;
    delete status;
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    QString str;
    str.sprintf("%7.3f",get_ps(arg1));
    ui->lineEdit->setText(str);
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    double ps;
    QString str;
    ps=arg1.toDouble();
    ps = get_altu(ps);
    str.sprintf("%5d",(int)ps) ;
    ui->highEdit->setText(str);
}
double MainWindow::get_altu(double ps)
{
   double high;
   if(ps>PbN)
   {
       high =-2000;
   }
    else if(ps>Pb1)
   {
        high = Tb0/B*(qExp(-B*R/gn*qLn(ps/Pb0))-1.0);
        if(high>0)
        {
            high += 0.5;
        }
        else
        {
            high -= 0.5;
        }
   }
   else if (ps>Pb2)
   {
        high = 11000  +(- R*Tb1/gn*qLn(ps/Pb1)+0.5);
   }
   else
   {
       high = 20000;
   }
    return high;
}
double MainWindow::get_ps(int h)
{
    double ps;
    if(h<-2000)
    {
        ps = PbN;
    }
    else if(h<11000)
    {
        ps = Pb0*qPow(((h*B)+Tb0)/Tb0,-gn/R/B);
    }
    else if(h<20000)
    {
        ps = Pb1*qExp(-gn/Tb1/R*(h-11000));
    }
    else
    {
        ps  = Pb2;
    }
    return ps;
}

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

}

void MainWindow::close_serial()
{
    if (serial->isOpen())
        serial->close();
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionSetup->setEnabled(true);
    showStatusMessage(tr("已断开"));
    received = false;
    cycle = 0;
    rect_queue.clear();
    cycque.clear();
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("关于高度传感器显示程序V1.0.0"),
                       tr("高度传感器显示程序基于Qt5.5.1开发，将海拔高度(m)转换为压力(kPa)并显示，"
                          "通过接收高度传感器串行信号，显示高度传感器2路海拔高度结果，并与给定的海拔高度进行"
                          "比较作出是否符合要求的结论，检测高度传感器串行信号的更新率和完整性。"));

}

void MainWindow::showStatusMessage(const QString &message)
{
    status->setText(message);
}

void MainWindow::readData()
{
    QByteArray data = serial->readAll();
    cnt++;
    if(data.size()==11 )
    {
        rect_queue.push_back(data);
        if(!received)
        {
            gettimeofday(&st,NULL);
            received = true;
        }
        else
        {
            gettimeofday(&ed,NULL);
            cycle =(ed.tv_sec-st.tv_sec)*1000000L+(ed.tv_usec-st.tv_usec);
            cycque.push_back(cycle);
            st.tv_sec = ed.tv_sec;
            st.tv_usec = ed.tv_usec;
        }
     qDebug()<<cycle/1000.0<<"ms,"<<cnt<<","<<data.size()<<","<<(quint8)data.at(9);
    }
}

void MainWindow::clkout()
{
   QByteArray data;
   qint64 sum;
   qint32 num;
   if(received)
   {
       sum = 0;
        num = rect_queue.size();
        for(qint32 i = 0;i<num;i++)
        {
            data = rect_queue.first();
            rect_queue.pop_front();
    //         qDebug()<<rect_queue.size()<<","<<(quint8)data[0]<<","<<(quint8)data[1];
        }
        num = cycque.size();
        for(qint32 i = 0;i<num;i++)
        {
            sum += cycque.first();
            cycque.pop_front();
        }
        qDebug()<<(float)sum/num/1000.0<<"ms";
   }
}
