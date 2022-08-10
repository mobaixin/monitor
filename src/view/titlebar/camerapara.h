#ifndef CAMERAPARA_H
#define CAMERAPARA_H

#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QGridLayout>
#include <QRadioButton>

#include "src/view/common/myslider.h"

class CameraPara : public QDialog
{
    Q_OBJECT
public:
    explicit CameraPara(QWidget *parent = nullptr);
    ~CameraPara();

    // 初始化组件
    void setWidgetUi();

    // 设置组件样式
    void setWidgetStyle();

    // 设置初始数据
    void setData();

private:
    void defaultBtnClick(); // 默认
    void cancelBtnClick();  // 取消
    void confirmBtnClick(); // 确认

    void updateExposeTime(int value);   // 更新曝光时间
    void updateCameraGain(int value);   // 更新增益值

private:
    QLabel *m_exposeTimeLab;    // 曝光时间标识
    QLabel *m_cameraGainLab;    // 增益值标识
//    QLabel *m_widDynamicLab;

    MySlider *m_exposeTimeSlider;   // 曝光时间滑动条
    MySlider *m_cameraGainSlider;   // 增益值滑动条
//    MySlider *m_widDynamicSlider;

    QPushButton *m_defaultBtn;  // 默认
    QPushButton *m_cancelBtn;   // 取消
    QPushButton *m_confirmBtn;  // 确认

    QGridLayout *m_mainLayout;  // 总体布局
    QHBoxLayout *m_btnLayout;   // 按钮布局
};

#endif // CAMERAPARA_H
