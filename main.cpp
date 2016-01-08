///@brief 高度传感器显示程序
///
/// 高度传感器显示程序，项目altimeter，项目框架由Qt-Creater自动产生\n
/// 其功能是将海拔高度(m)转换为压力(kPa)并显示，通过接收高度传感器\n
/// 串行信号，显示高度传感器2路海拔高度结果，并与给定的海拔高度进行\n
/// 比较作出是否符合要求的结论，检测高度传感器串行信号的更新率和完整性\n
/// 本项目是基于Qt跨平台C++图形用户界面应用程序开发框架\n
/// @file main.cpp
/// @version V1.0.0
/// @author 沈晓飞 380406785@@qq.com
/// @date   2015-12-30

#include "mainwindow.h"
#include <QApplication>

///@brief 高度传感器显示主程序
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setFixedSize(407,300);
    w.show();

    return a.exec();
}
