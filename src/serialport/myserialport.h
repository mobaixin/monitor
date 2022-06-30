#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

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

    void receiveInfo();

signals:

private:
    QSerialPort *m_serialPort;
    QStringList m_portNameList;

};

#endif // SERIALPORT_H
