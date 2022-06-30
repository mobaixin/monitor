#include <QDebug>

#include "myserialport.h"

MySerialPort *MySerialPort::getInstance(QObject *parent)
{
    static MySerialPort *instance = nullptr;
    if (nullptr == instance) {
        instance = new MySerialPort(parent);
    }
    return instance;
}

MySerialPort::MySerialPort(QObject *parent)
    : QObject(parent)
{
    m_serialPort = new QSerialPort();
    m_portNameList = getPortNameList();

    openPort();
}

MySerialPort::~MySerialPort()
{
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
    }

    m_serialPort->deleteLater();
}

QStringList MySerialPort::getPortNameList()
{
    QStringList serialPortName;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        serialPortName.append(info.portName());
        qDebug() << "Name : " << info.portName();
        qDebug() << "Description : " << info.description();
        qDebug() << "Manufacturer: " << info.manufacturer();
        qDebug() << "Serial Number: " << info.serialNumber();
        qDebug() << "System Location: " << info.systemLocation();
    }

    return serialPortName;
}

void MySerialPort::openPort()
{
    if (m_serialPort->isOpen()) {
        m_serialPort->clear();
        m_serialPort->close();
    }

    m_serialPort->setPortName(m_portNameList[0]);

    if (!m_serialPort->open(QIODevice::ReadWrite)) {
        qDebug() << "串口打开失败";
        return ;
    }
    qDebug() << "串口打开成功";

    m_serialPort->setBaudRate(QSerialPort::Baud115200, QSerialPort::AllDirections); // 设置波特率和读写方向
    m_serialPort->setDataBits(QSerialPort::Data8);              // 数据位为8位
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);   // 无流控制
    m_serialPort->setParity(QSerialPort::NoParity);             // 无校验位
    m_serialPort->setStopBits(QSerialPort::OneStop);            // 一位停止

    connect(m_serialPort, &QSerialPort::readyRead, this, &MySerialPort::receiveInfo);
}

void MySerialPort::receiveInfo()
{
    QByteArray info = m_serialPort->readAll();

    qDebug() << "receive info: " << info;
}




