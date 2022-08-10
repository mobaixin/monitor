#ifndef SYSSETTING_H
#define SYSSETTING_H

#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QGridLayout>
#include <QRadioButton>
#include <QPushButton>
#include <QButtonGroup>

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
    void changeTimeBtnClick();          // 修改时间
    void ioSettingsBtnClick();          // IO设置
    void cameraParaBtnClick();          // 相机参数
    void closeSetBtnClick();            // 关闭

    void updateMoldDelay(int value);    // 更新检模检测延时
    void updateMoldTimes(int value);    // 更新检模重检次数
    void updateProdDelay(int value);    // 更新产品检测延时
    void updateProdTimes(int value);    // 更新产品重检次数
    void updateProdDetect(bool checked);    // 更新启用产品检测

    void updateCameraCount();           // 更新相机数

    void updateDisPlay(bool isShowProd);    // 更新显示产品检测

private:
    QLabel *m_moldDelayLab;         // 检模检测延时
    QLabel *m_moldTimesLab;         // 检模重检次数
    QLabel *m_prodDelayLab;         // 产品检测延时
    QLabel *m_prodTimesLab;         // 产品重检次数
    QLabel *m_prodDetectLab;        // 产品检测启用
    QLabel *m_cameraCountLab;       // 相机数

    MySlider *m_moldDelaySlider;    // 检模检测延时滑动条
    MySlider *m_moldTimesSlider;    // 检模重检次数滑动条
    MySlider *m_prodDelaySlider;    // 产品检测延时滑动条
    MySlider *m_prodTimesSlider;    // 产品重检次数滑动条

    QRadioButton *m_prodDetectBtn;  // 产品检测启用按钮

    // 相机数
    QList<QPushButton *> m_cameraBtnList;
    QButtonGroup *m_cameraBtnGroup;

    QPushButton *m_changeTimeBtn;   // 修改时间
    QPushButton *m_ioSettingsBtn;   // IO设置
    QPushButton *m_cameraParaBtn;   // 相机参数
    QPushButton *m_closeSetBtn;     // 关闭

    QGridLayout *m_sysSetLayout;    // 设置布局
    QHBoxLayout *m_camBtnLayout;    // 相机数布局
    QHBoxLayout *m_btnLayout;       // 底部按钮布局
    QVBoxLayout *m_mainLayout;      // 总体布局

    CameraPara *m_cameraPara;       // 相机参数界面
    IOSetting *m_ioSetting;         // IO设置界面
};

#endif // SYSSETTING_H
