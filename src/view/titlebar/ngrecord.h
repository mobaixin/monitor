#ifndef NGRECORD_H
#define NGRECORD_H

#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QTextEdit>

#include "src/database/mydatabase.h"

class NGRecord : public QDialog
{
    Q_OBJECT
public:
    explicit NGRecord(QWidget *parent = nullptr);

    // 初始化组件
    void setWidgetUi();

    // 设置组件样式
    void setWidgetStyle();

    // 设置初始数据
    void setData();

    // 添加NG记录
    void addNgRecord(NGRecordData ngData);

private:
    void resetBtnClick();
    void optRecordBtnClick();
    void closeBtnClick();

    // 获取模型数据
    void getModelData();

private:
    QTableView *m_ngTableView;
    QStandardItemModel *m_ngModel;

    QTextEdit *m_recordText;

    QLabel *m_resultLab;

    QPushButton *m_resetBtn;
    QPushButton *m_optRecordBtn;
    QPushButton *m_closeBtn;

    QHBoxLayout *m_mainLayout;
    QVBoxLayout *m_rightLayout;

    int m_okTotalNum;
    int m_ngTotalNum;

};

#endif // NGRECORD_H
