#ifndef MYSELECTFRAME_H
#define MYSELECTFRAME_H

#include <QObject>
#include <QWidget>
#include <QFrame>
#include <QRadioButton>
#include <QLabel>
#include <QList>
#include <QHBoxLayout>

class MySelectFrame : public QWidget
{
    Q_OBJECT
public:
    MySelectFrame(QWidget *parent = nullptr, int frameId = -1, int btnNum = 2);

    // 初始化组件
    void setWidgetUi();

    // 设置组件样式
    void setWidgetStyle();

    // 设置初始数据
    void setData();

    // 设置框名
    void setFrameName(QString name);

    // 设置选项文字
    void setOptionText(QStringList btnNameList);

    // 设置选择项
    void setSelectNum(int num);

    // 获取选择项
    int getSelectNum();

private:
    void radioButtonClick();

signals:
    void valueChange(int frameId, int selectNum);

private:
    QLabel *m_frameNameLab;
    QFrame *m_frame;
    QList<QRadioButton *> m_radioBtnList;
    QHBoxLayout *m_frameLayout;
    QHBoxLayout *m_mainLayout;

    int m_frameId;
    int m_radioBtnNum;
    int m_selectNum;
};

#endif // MYSELECTFRAME_H
