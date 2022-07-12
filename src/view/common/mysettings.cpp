#include <QFileInfo>

#include "mysettings.h"

MySettings *MySettings::getInstance()
{
    static MySettings *instance = nullptr;
    if (nullptr == instance) {
        instance = new MySettings();
    }
    return instance;
}

MySettings::MySettings(QObject *parent)
    : QObject(parent)
{
    QMutexLocker lockData(&m_mutex);

    m_iniFilePath = QCoreApplication::applicationDirPath() + "/data/monitor.ini";

    QFileInfo fileInfo(m_iniFilePath);
    bool isExistFile = fileInfo.isFile();

    m_settings = new QSettings(m_iniFilePath, QSettings::IniFormat);

    if (!isExistFile) {
        setInitValue();
    }
}

void MySettings::setValue(QString section, QString key, QString value)
{
    m_settings->setValue(QString("%1/%2").arg(section).arg(key), value);
}

QString MySettings::getValue(QString section, QString key)
{
    return m_settings->value(QString("%1/%2").arg(section).arg(key)).toString();
}

void MySettings::delValue(QString section, QString key)
{
    m_settings->remove(QString("%1/%2").arg(section).arg(key));
}

void MySettings::setInitValue()
{
    // 图形设置
    m_settings->setValue(QString("%1/%2").arg(ShapeSection).arg("accuracy"), "30");
    m_settings->setValue(QString("%1/%2").arg(ShapeSection).arg("pixel"),    "300");

    // 系统设置
    m_settings->setValue(QString("%1/%2").arg(SysSection).arg("moldDelay"),  "6");
    m_settings->setValue(QString("%1/%2").arg(SysSection).arg("moldTimes"),  "4");
    m_settings->setValue(QString("%1/%2").arg(SysSection).arg("prodDelay"),  "4");
    m_settings->setValue(QString("%1/%2").arg(SysSection).arg("prodTimes"),  "2");
    m_settings->setValue(QString("%1/%2").arg(SysSection).arg("prodDetect"), "1");

    // 相机参数
    m_settings->setValue(QString("%1/%2").arg(CameraSection).arg("exposeTime"), "1000");
    m_settings->setValue(QString("%1/%2").arg(CameraSection).arg("cameraGain"), "24");

    // IO设置
    setIOInitValue();
}

void MySettings::setIOInitValue()
{
    QStringList ioFrameValueList;
    ioFrameValueList << "1" << "0" << "0" << "1" << "0" << "0" << "1" << "0" << "0";
    ioFrameValueList << "1" << "0" << "0" << "0" << "0";
    ioFrameValueList << "0" << "0" << "0" << "0" << "1";
    ioFrameValueList << "0" << "0" << "1" << "0";
    ioFrameValueList << "0" << "0" << "0" << "0" << "0";

    for (int i = 0; i < 28; i++) {
        m_settings->setValue(QString("%1/frameList%2").arg(IOSetSection).arg(i), ioFrameValueList[i]);
    }

    m_settings->setValue(QString("%1/%2").arg(IOSetSection).arg("periodSig"),     "0");
    m_settings->setValue(QString("%1/%2").arg(IOSetSection).arg("aseismicLevel"), "3");
    m_settings->setValue(QString("%1/%2").arg(IOSetSection).arg("maxSimpleNum"),  "20");
    m_settings->setValue(QString("%1/%2").arg(IOSetSection).arg("maxReDeteNum"),  "10");
}
