#ifndef SYSSETTING_H
#define SYSSETTING_H

#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QGridLayout>
#include <QRadioButton>

#include "src/view/common/myslider.h"
#include "src/view/titlebar/camerapara.h"
#include "src/view/titlebar/iosetting.h"

class SysSetting : public QDialog
{
    Q_OBJECT
public:
    explicit SysSetting(QWidget *parent = nullptr);
    ~SysSetting();

    // 初始化组件
    void setWidgetUi();

    // 设置组件样式
    void setWidgetStyle();

    // 设置初始数据
    void setData();

private:
    void changeTimeBtnClick();
    void ioSettingsBtnClick();
    void cameraParaBtnClick();
    void closeSetBtnClick();

    void updateMoldDelay(int value);
    void updateMoldTimes(int value);
    void updateProdDelay(int value);
    void updateProdTimes(int value);
    void updateProdDetect(bool checked);

private:
    QLabel *m_moldDelayLab;
    QLabel *m_moldTimesLab;
    QLabel *m_prodDelayLab;
    QLabel *m_prodTimesLab;
    QLabel *m_prodDetectLab;

    MySlider *m_moldDelaySlider;
    MySlider *m_moldTimesSlider;
    MySlider *m_prodDelaySlider;
    MySlider *m_prodTimesSlider;

    QRadioButton *m_prodDetectBtn;

    QPushButton *m_changeTimeBtn;
    QPushButton *m_ioSettingsBtn;
    QPushButton *m_cameraParaBtn;
    QPushButton *m_closeSetBtn;

    QGridLayout *m_sysSetLayout;

    CameraPara *m_cameraPara;
    IOSetting *m_ioSetting;
};

#endif // SYSSETTING_H
