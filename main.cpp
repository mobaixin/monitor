﻿#include "src/view/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
//    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    QApplication a(argc, argv);
    MainWindow::getInstance();
    MainWindow::getInstance()->show();
    return a.exec();
}
