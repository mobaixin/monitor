#ifndef OPTRECORD_H
#define OPTRECORD_H

#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>

class OptRecord : public QDialog
{
    Q_OBJECT
public:
    explicit OptRecord(QWidget *parent = nullptr);

    // 初始化组件
    void setWidgetUi();

    // 设置组件样式
    void setWidgetStyle();

    // 设置初始数据
    void setData();

    // 添加操作记录
    static void addOptRecord(QString optLog = "");

private:
    void prevPageBtnClick();
    void nextPageBtnClick();
    void closeBtnClick();

    // 获取模型数据
    void getModelData();

private:
    QPushButton *m_prevPageBtn;
    QPushButton *m_nextPageBtn;
    QPushButton *m_closeBtn;

    QTableView *m_optTableView;
    QStandardItemModel *m_optModel;

    QHBoxLayout *m_btnLayout;
    QVBoxLayout *m_mainLayout;

    int m_pageValue = 17;
};

#endif // OPTRECORD_H
