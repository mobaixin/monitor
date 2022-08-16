#ifndef MYSETTINGS_H
#define MYSETTINGS_H

#include <QObject>
#include <QSettings>
#include <QMutexLocker>
#include <QApplication>
#include <QDir>

// all sections
const QString ShapeSection  = "ShapeSetting";
const QString SysSection    = "SysSetting";
const QString CameraSection = "CameraSetting";
const QString IOSetSection  = "IOSetting";
const QString DetectSection = "DetectSetting";

// all keys
const QString AccuracyKey   = "accuracy";
const QString PixelKey      = "pixel";

const QString MoldDelayKey  = "moldDelay";
const QString MoldTimesKey  = "moldTimes";
const QString ProdDelayKey  = "prodDelay";
const QString ProdTimesKey  = "prodTimes";
const QString ProdDetectKey   = "prodDetect";
const QString CameraCountsKey = "cameraCounts";

const QString ExposeTimeKey = "exposeTime";
const QString CameraGainKey = "cameraGain";

const QString FrameListKey     = QString("frameList%1");
const QString PeriodSigKey     = "periodSig";
const QString AseismicLevelKey = "aseismicLevel";
const QString MaxSampleNumKey  = "maxSampleNum";
const QString MaxReDeteNumKey  = "maxReDeteNum";

const QString DetectNGTimes = "detectNGTimes";
const QString DetectOKTimes = "detectOKTimes";

class MySettings : public QObject
{
    Q_OBJECT
public:
    // 单例
    static MySettings *getInstance();

    explicit MySettings(QObject *parent = nullptr);

    // 设置配置项
    void setValue(QString section, QString key, QString value);

    // 读取配置项
    QString getValue(QString section, QString key);

    // 删除配置项
    void delValue(QString section, QString key);

    // 初始化配置项
    void setInitValue();

    // 初始化IO设置配置项
    void setIOInitValue();

signals:

private:
    QString m_iniFilePath;  // 配置文件的路径
    QSettings *m_settings;  // 配置

    QMutex m_mutex;         // 互斥锁

};

#endif // MYSETTINGS_H
