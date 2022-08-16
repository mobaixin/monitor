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
    void frameValueChange(int frameId, int selectNum);  // 选项值变化

    void updatePeriodSig(int value);        // 更新周期信号输出
    void updateAseismicLevel(int value);    // 更新抗震级别
    void updateMaxSampleNum(int value);     // 更新最大样本数
    void updateMaxReDeteNum(int value);     // 更新最大重检次数

    void restartBtnClick();     // 重启
    void passwordBtnClick();    // 按钮子密码
    void ipsetBtnClick();       // 相机IP配置
    void resetBtnClick();       // 恢复默认
    void testModeBtnClick();    // 测试模式
    void saveBtnClick();        // 保存


private:
    QList<MySelectFrame *> m_frameList; // 设置项列表
    QList<QLabel *> m_labelList;        // 底部设置项标识
    QList<MySlider *> m_sliderList;     // 底部设置项滑动条

    QPushButton *m_restartBtn;  // 重启
    QPushButton *m_passwordBtn; // 按钮子密码
    QPushButton *m_ipsetBtn;    // 相机IP配置
    QPushButton *m_resetBtn;    // 恢复默认
    QPushButton *m_testModeBtn; // 测试模式
    QPushButton *m_saveBtn;     // 保存

    QGridLayout *m_gridLayout1; // 布局
    QGridLayout *m_gridLayout2;
    QHBoxLayout *m_hboxLayout3;
    QHBoxLayout *m_hboxLayout4;
    QHBoxLayout *m_hboxLayout5;
    QHBoxLayout *m_hboxLayout6;
    QVBoxLayout *m_mainLayout;  // 总体布局

    CameraIpSet *m_cameraIpSet; // 相机IP配置界面
};

#endif // IOSETTING_H
