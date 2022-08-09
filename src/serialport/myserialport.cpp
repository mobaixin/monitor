#include <QDebug>

#include "myserialport.h"
#include "src/view/mainwindow.h"
#include "src/view/common/mysettings.h"

#include <opencv2\opencv.hpp>

#pragma comment (lib, "Advapi32.lib")

#include <stdlib.h>
#if _MSC_VER>=1900
#include "stdio.h"
_ACRTIMP_ALT FILE* __cdecl __acrt_iob_func(unsigned);
#ifdef __cplusplus
extern "C"
#endif
FILE* __cdecl __iob_func(unsigned i) {
    return __acrt_iob_func(i);
}
#endif /* _MSC_VER>=1900 */

#include "usbdevice.h"
#include "usb2gpio.h"


#if _MSC_VER >=1600    // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif

static DEVICE_INFO m_devInfo;

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

    m_timer = new QTimer(this);
    m_timer->start(1000);
//    connect(m_timer, &QTimer::timeout, this, &MySerialPort::readTimesInfo);
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


    // 扫描查找设备
    m_ret = USB_ScanDevice(m_devHandle);
    if (m_ret <= 0) {
        qDebug() << "No device connected!";
        return ;
    } else {
        qDebug() << "Found devices:";

        for (int i = 0; i < m_ret; i++) {
            qDebug() << QString().sprintf("%08X", m_devHandle[i]);
        }
    }

    // 打开设备
    m_state = USB_OpenDevice(m_devHandle[0]);
    if (!m_state) {
        qDebug() << "Open device error!";
        return ;
    }

    // 初始化输出端口
    setOutputPort(m_devHandle[0], OutputMask, 0);

    // 初始化输入端口
    setInputPort(m_devHandle[0], InputMask, 0);

    writeInfo(OutputMask, OutputMask);
}

void MySerialPort::closePort(int devHandle)
{
    USB_CloseDevice(devHandle);
}

int MySerialPort::setInputPort(int devHandle, unsigned int pinMask, unsigned char puPd)
{
    return GPIO_SetInput(devHandle, pinMask, puPd);
}

int MySerialPort::setOutputPort(int devHandle, unsigned int pinMask, unsigned char puPd)
{
    return GPIO_SetOutput(devHandle, pinMask, puPd);
}

int MySerialPort::writeInfo(unsigned int pinMask, unsigned int pinValue)
{
//    return GPIO_Write(m_devHandle[0], pinMask, pinValue);

    GPIO_Write(m_devHandle[0], pinMask, pinValue);

    unsigned int value = 0x0000;
    GPIO_Read(m_devHandle[0], OutputMask, &value);
    qDebug() << "get output value: " << QString().sprintf("READ DATA:%04X", value);

    return 0;
}

int MySerialPort::readInfo(int devHandle, unsigned int pinMask, unsigned int *pinValue)
{
    int res = GPIO_Read(devHandle, pinMask, pinValue);
    qDebug() << QString().sprintf("READ DATA:%04X", *pinValue);

    return res;
}

void MySerialPort::receiveInfo()
{
    QByteArray info = m_serialPort->readAll();

    qDebug() << "receive info: " << info;
}

void MySerialPort::readTimesInfo()
{
    // 开模
    unsigned int openMoldSig = 0x0080;

    // 顶退
    unsigned int topBackSig  = 0x0040;

    // 获取信号设置
    int openMoldFlag = MySettings::getInstance()->getValue(IOSetSection, FrameListKey.arg(0)).toInt();
    int topBackFlag  = MySettings::getInstance()->getValue(IOSetSection, FrameListKey.arg(3)).toInt();

    // 获取GPIO信号
    unsigned int value = 0;
    int res = readInfo(m_devHandle[0], InputMask, &value);

    if (res != GPIO_SUCCESS) {
        return ;
    } else {
        qDebug() << "readTimesInfo: " << value;

        for (int i = 0; i < 4; i++) {
            if ((value & (0x40 << i)) != 0x00) {
                qDebug() << QString("P[%1]为高电平").arg(i);
            } else {
                qDebug() << QString("P[%1]为低电平").arg(i);
            }
        }

        // TODO: 根据GPIO信号判断相机和场景

        // 产品
        if ((((value & openMoldSig) != 0x0000) && openMoldFlag == 1) ||
            (((value & openMoldSig) == 0x0000) && openMoldFlag == 0)) {
            MainWindow::getInstance()->autoDetectImage(1, 2);   
        // 检模
        } else if ((((value & topBackSig) != 0x0000) && topBackFlag == 1) ||
                   (((value & topBackSig) == 0x0000) && topBackFlag == 0)) {
            MainWindow::getInstance()->autoDetectImage(1, 1);
        }

    }

}




