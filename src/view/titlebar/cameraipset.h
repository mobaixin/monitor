#ifndef CAMERAIPSET_H
#define CAMERAIPSET_H

#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>

#include "src/database/mydatabase.h"

class MyIpInputDialog;

class CameraIpSet : public QDialog
{
    Q_OBJECT
public:
    explicit CameraIpSet(QWidget *parent = nullptr);

    // 初始化组件
    void setWidgetUi();

    // 设置组件样式
    void setWidgetStyle();

    // 设置初始数据
    void setData();

private:
    void freshBtnClick();       // 刷新
    void changeIpBtnClick();    // 修改IP
    void autoGetIpBtnClick();   // 自动分配IP
    void confirmBtnClick();     // 确认
    void cancelBtnClick();      // 取消

    // 获取模型数据
    void getModelData();

private:
    QTableView *m_ipTableView;      // 相机IP 显示view
    QStandardItemModel *m_ipModel;  // 相机IP 数据model

    QPushButton *m_freshBtn;        // 刷新
    QPushButton *m_changeIpBtn;     // 修改IP
    QPushButton *m_autoGetIpBtn;    // 自动分配IP
    QPushButton *m_confirmBtn;      // 确认
    QPushButton *m_cancelBtn;       // 取消

    QHBoxLayout *m_mainLayout;      // 总体布局
    QVBoxLayout *m_rightLayout;     // 右侧布局

    QList<CameraIPData> m_cameraIPDataList; // 相机IP数据列表

    MyIpInputDialog *m_myIpInputDialog; // 修改IP对话框
};


class MyIpInputDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MyIpInputDialog(QWidget *parent = nullptr);

    // 初始化组件
    void setWidgetUi();

    // 设置组件样式
    void setWidgetStyle();

    // 设置初始数据
    void setData();

signals:
    // 获取输入的IP 信号
    void getValues(QString value);

private:
    void cancelBtnClick();  // 取消
    void confirmBtnClick(); // 确认

private:
    QLabel *m_captionLab;       // 标题
    QLineEdit *m_ipInputEdit;   // IP输入框

    QPushButton *m_cancelBtn;   // 取消
    QPushButton *m_confirmBtn;  // 确认

    QHBoxLayout *m_btnLayout;   // 按钮布局
    QVBoxLayout *m_mainLayout;  // 总体布局

};

#endif // CAMERAIPSET_H
