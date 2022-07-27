#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <string.h>
#include <QTimer>

//#pragma comment(lib, "D:\\Documents\\QTProjects\\monitor\\src\\serialport\\lib\\Lucero.lib")

const unsigned int InputMask  = 0x03C0;
const unsigned int OutputMask = 0x003F;

// 可顶针
const unsigned int ThimbleMask      = 0x0001;
const unsigned int CanThimbleValue  = 0x0001;
const unsigned int NotThimbleValue  = 0x0000;

// 可合模
const unsigned int ClampMoldMask      = 0x0002;
const unsigned int CanClampMoldValue  = 0x0002;
const unsigned int NotClampMoldValue  = 0x0000;

class MySerialPort : public QObject
{
    Q_OBJECT
public:
    static MySerialPort *getInstance(QObject *parent = nullptr);

    explicit MySerialPort(QObject *parent = nullptr);
    ~MySerialPort();

    // 获取所有可用的串口列表
    QStringList getPortNameList();

    // 打开串口
    void openPort();

    // 关闭串口
    void closePort(int devHandle);

    // 设置输入端口 pinMask,选择引脚; puPd,配置引脚模式
    int setInputPort(int devHandle, unsigned int pinMask, unsigned char puPd);

    // 设置输出端口
    int setOutputPort(int devHandle, unsigned int pinMask, unsigned char puPd);

    // 写入信息数据 pinMask,选择引脚; pinValue,设置电平
    int writeInfo(unsigned int pinMask, unsigned int pinValue);

    // 读取信息数据 pinMask,选择引脚; *pinValue,读取电平
    int readInfo(int devHandle,unsigned int pinMask, unsigned int *pinValue);

    void receiveInfo();

    void readTimesInfo();

private:
    QSerialPort *m_serialPort;
    QStringList m_portNameList;


    int m_devHandle[10];
    bool m_state;
    int m_ret;
    unsigned int m_pinValue;

    QTimer *m_timer;

};

#endif // SERIALPORT_H
