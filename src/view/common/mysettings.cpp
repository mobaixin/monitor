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

}
