#ifndef MYSETTINGS_H
#define MYSETTINGS_H

#include <QObject>
#include <QSettings>

class MySettings : public QObject
{
    Q_OBJECT
public:
    static MySettings *getInstance();

    explicit MySettings(QObject *parent = nullptr);

signals:

private:
    QString m_iniFilePath;

};

#endif // MYSETTINGS_H
