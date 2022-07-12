#ifndef CAMERAIPSET_H
#define CAMERAIPSET_H

#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>

#include "src/database/mydatabase.h"

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
    void freshBtnClick();
    void changeIpBtnClick();
    void autoGetIpBtnClick();
    void confirmBtnClick();
    void cancelBtnClick();

    // 获取模型数据
    void getModelData();

private:
    QTableView *m_ipTableView;
    QStandardItemModel *m_ipModel;

    QPushButton *m_freshBtn;
    QPushButton *m_changeIpBtn;
    QPushButton *m_autoGetIpBtn;
    QPushButton *m_confirmBtn;
    QPushButton *m_cancelBtn;

    QHBoxLayout *m_mainLayout;
    QVBoxLayout *m_rightLayout;

    QList<CameraIPData> m_cameraIPDataList;
};

#endif // CAMERAIPSET_H
