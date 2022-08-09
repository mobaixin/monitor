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
