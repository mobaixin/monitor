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
    void defaultBtnClick();
    void cancelBtnClick();
    void confirmBtnClick();

    void updateExposeTime(int value);
    void updateCameraGain(int value);

private:
    QLabel *m_exposeTimeLab;
    QLabel *m_cameraGainLab;
//    QLabel *m_widDynamicLab;

    MySlider *m_exposeTimeSlider;
    MySlider *m_cameraGainSlider;
//    MySlider *m_widDynamicSlider;

    QPushButton *m_defaultBtn;
    QPushButton *m_cancelBtn;
    QPushButton *m_confirmBtn;

    QGridLayout *m_mainLayout;
    QHBoxLayout *m_btnLayout;
};

#endif // CAMERAPARA_H
