#ifndef MYSETTINGS_H
#define MYSETTINGS_H

#include <QObject>
#include <QSettings>
#include <QMutexLocker>
#include <QApplication>
#include <QDir>

const QString ShapeSection  = "ShapeSetting";
const QString SysSection    = "SysSetting";
const QString CameraSection = "CameraSetting";
const QString IOSetSection  = "IOSetting";

class MySettings : public QObject
{
    Q_OBJECT
public:
    static MySettings *getInstance();

    explicit MySettings(QObject *parent = nullptr);

    void setValue(QString section, QString key, QString value);

    QString getValue(QString section, QString key);

    void delValue(QString section, QString key);

    void setInitValue();

    void setIOInitValue();

signals:

private:
    QString m_iniFilePath;
    QSettings *m_settings;

    QMutex m_mutex;

};

#endif // MYSETTINGS_H
