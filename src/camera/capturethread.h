﻿#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H
#include <QThread>
#include <QImage>


class CaptureThread : public QThread
{
    Q_OBJECT
public:
    explicit CaptureThread(QObject *parent = 0, int cameraId = 1);

public:
    void run();
    void stream();
    void pause();
    void stop();

    bool quit;


signals:
    void captured(QImage img, int cameraId);
private:
    bool pause_status;

    int m_cameraId;

    QVector<QRgb> grayColourTable;
    QVector<QRgb> ColourTable;

public slots:

};

#endif // CAPTURETHREAD_H
