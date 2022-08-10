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
    void prevPageBtnClick();    // 上一页
    void nextPageBtnClick();    // 下一页
    void closeBtnClick();       // 关闭

    // 获取模型数据
    void getModelData();

private:
    QPushButton *m_prevPageBtn; // 上一页
    QPushButton *m_nextPageBtn; // 下一页
    QPushButton *m_closeBtn;    // 关闭

    QTableView *m_optTableView;     // 操作记录显示view
    QStandardItemModel *m_optModel; // 操作记录数据model

    QHBoxLayout *m_btnLayout;   // 按钮布局
    QVBoxLayout *m_mainLayout;  // 总体布局

    int m_pageValue = 17;       // 每页显示的记录条数
};

#endif // OPTRECORD_H
