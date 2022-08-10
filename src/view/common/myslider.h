#ifndef MYSLIDER_H
#define MYSLIDER_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QSlider>

class MySlider : public QWidget
{
    Q_OBJECT
public:
    explicit MySlider(QWidget *parent = nullptr);
    virtual ~MySlider();

    // 初始化组件
    void setWidgetUi();

    // 设置组件样式
    void setWidgetStyle();

    // 设置初始数据
    void setData();

    // 设置滑动条步长
    void setStep(int step);

    // 设置数值范围
    void setValueRange(int min, int max);

    // 设置输入框尺寸
    void setEditSize(int width, int height = 20);

    // 设置滑动条尺寸
    void setSliderSize(int width, int height);

    // 设置自动填充输入框的值
    void setAutoSetEditValue(bool isAuto);

    // 设置数值
    void setValue(int value, bool isSetEdit = true);

    // 单独设置输入框数值
    void setEditValue(QString value);

    // 获取数值
    QString getEditValue();

    // 设置输入框状态
    void setEditReadOnly(bool isReadOnly);

public:
    // 滑动条
    QSlider *m_slider;

signals:
    // 数值变化信号
    void valueChange(int value);

private:
    // 按钮响应事件
    void addBtnClick();
    void subBtnClick();

    // 设置输入框的值
    void setLineValue(int value);

    // 设置滑动条的值
    void setSliderValue();

private:
    int m_sliderStep;           // 滑动条步长
    bool m_isAutoSetEditValue;  // 是否自动设置输入框的值

    QLineEdit *m_lineEdit;      // 输入框

    QPushButton *m_addBtn;      // 按钮＋
    QPushButton *m_subBtn;      // 按钮-

    QHBoxLayout *m_slideLayout; // 布局
};

#endif // MYSLIDER_H
