#ifndef IOSETTING_H
#define IOSETTING_H

#include <QObject>
#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include "src/view/common/myslider.h"
#include "src/view/common/myselectframe.h"
#include "src/view/titlebar/cameraipset.h"

class IOSetting : public QDialog
{
    Q_OBJECT
public:
    explicit IOSetting(QWidget *parent = nullptr);

    // 初始化组件
    void setWidgetUi();

    // 设置组件样式
    void setWidgetStyle();

    // 设置初始数据
    void setData();

private:

    void restartBtnClick();
    void passwordBtnClick();
    void ipsetBtnClick();
    void resetBtnClick();
    void testModeBtnClick();
    void saveBtnClick();


private:
    QList<MySelectFrame *> m_frameList;
    QList<QLabel *> m_labelList;
    QList<MySlider *> m_sliderList;

    QPushButton *m_restartBtn;
    QPushButton *m_passwordBtn;
    QPushButton *m_ipsetBtn;
    QPushButton *m_resetBtn;
    QPushButton *m_testModeBtn;
    QPushButton *m_saveBtn;

    QGridLayout *m_gridLayout1;
    QGridLayout *m_gridLayout2;
    QHBoxLayout *m_hboxLayout3;
    QHBoxLayout *m_hboxLayout4;
    QHBoxLayout *m_hboxLayout5;
    QHBoxLayout *m_hboxLayout6;
    QVBoxLayout *m_mainLayout;

    CameraIpSet *m_cameraIpSet;
};

#endif // IOSETTING_H
