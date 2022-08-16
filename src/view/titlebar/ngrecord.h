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
#include "src/view/titlebar/optrecord.h"

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

    // 添加NG文字记录
    static void addNgTextRecord(QString ngText);

    // 关闭事件
    void closeEvent(QCloseEvent *event);

private:
    void resetBtnClick();       // 重置
    void optRecordBtnClick();   // 操作记录
    void closeBtnClick();       // 关闭

    // 获取模型数据
    void getModelData();

    // 获取NG文字记录
    QString getNGTextRecord();

    // 显示NG记录图片
    void showNgRecordImg(const QModelIndex &index);

private:
    QTableView *m_ngTableView;      // NG记录显示view
    QStandardItemModel *m_ngModel;  // NG记录数据model

    QTextEdit *m_recordTextEdit;    // 操作记录

    QLabel *m_resultLab;            // NG次数显示

    QPushButton *m_resetBtn;        // 重置
    QPushButton *m_optRecordBtn;    // 操作记录
    QPushButton *m_closeBtn;        // 关闭

    QHBoxLayout *m_mainLayout;      // 总体布局
    QVBoxLayout *m_rightLayout;     // 右侧布局

    QList<NGRecordData> m_recordDataList;   // NG记录数据列表

    OptRecord *m_optRecord;         // 操作记录界面

    bool m_imgAreaShowState;        // 相机视频流显示状态
};

#endif // NGRECORD_H
