#include <QDateTime>
#include <QDebug>

#include "optrecord.h"
#include "src/database/mydatabase.h"

#if _MSC_VER >=1600    // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif

OptRecord::OptRecord(QWidget *parent)
    : QDialog(parent)
{
    // 初始化组件
    setWidgetUi();

    // 设置组件样式
    setWidgetStyle();

    // 设置初始数据
    setData();
}

void OptRecord::setWidgetUi()
{
    m_prevPageBtn = new QPushButton(this);
    m_nextPageBtn = new QPushButton(this);
    m_closeBtn    = new QPushButton(this);

    m_optTableView = new QTableView(this);
    m_optModel     = new QStandardItemModel(this);

    m_btnLayout  = new QHBoxLayout();
    m_mainLayout = new QVBoxLayout(this);

    m_btnLayout->addWidget(m_prevPageBtn);
    m_btnLayout->addWidget(m_nextPageBtn);
    m_btnLayout->addWidget(m_closeBtn);
    m_btnLayout->setContentsMargins(0, 0, 0, 0);
    m_btnLayout->setSpacing(5);

    m_mainLayout->addLayout(m_btnLayout);
    m_mainLayout->addWidget(m_optTableView);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    m_mainLayout->setSpacing(10);

    this->setLayout(m_mainLayout);

    connect(m_prevPageBtn, &QPushButton::clicked, this, &OptRecord::prevPageBtnClick);
    connect(m_nextPageBtn, &QPushButton::clicked, this, &OptRecord::nextPageBtnClick);
    connect(m_closeBtn,    &QPushButton::clicked, this, &OptRecord::closeBtnClick);

}

void OptRecord::setWidgetStyle()
{
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    this->setFixedSize(800, 600);
    this->setWindowTitle("操作记录");

    m_prevPageBtn->setFixedHeight(40);
    m_nextPageBtn->setFixedHeight(40);
    m_closeBtn->setFixedHeight(40);
//    m_optTableView->setFixedSize(980, 530);

    QFont viewFont = this->font();
    viewFont.setPixelSize(15);

    // 按钮样式
    m_prevPageBtn->setFont(viewFont);
    m_nextPageBtn->setFont(viewFont);
    m_closeBtn->setFont(viewFont);

    m_prevPageBtn->setText("前一页");
    m_nextPageBtn->setText("后一页");
    m_closeBtn->setText("关闭");

    m_prevPageBtn->setFocusPolicy(Qt::NoFocus);
    m_nextPageBtn->setFocusPolicy(Qt::NoFocus);
    m_closeBtn->setFocusPolicy(Qt::NoFocus);

    // 表格样式
    m_optTableView->setFont(viewFont);
    m_optTableView->setAlternatingRowColors(true);

    m_optModel->setHorizontalHeaderLabels({" ID "," 时间 ", " 操作员 ", " 日志 "});
    m_optTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_optTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_optTableView->verticalHeader()->setHidden(true);
    m_optTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_optTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_optTableView->setModel(m_optModel);
}

void OptRecord::setData()
{
    getModelData();
    m_optTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_optTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_optTableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
}

void OptRecord::addOptRecord(QString optLog)
{
    QDateTime time   = QDateTime::currentDateTime();
    QString timeStr  = time.toString("yyyy-MM-dd HH:mm:ss");

    OptRecordData recordData;
    recordData.time = timeStr;
    recordData.optorName = "";
    recordData.optLog = optLog;

    MyDataBase::getInstance()->addOptRecordData(recordData);
}

void OptRecord::prevPageBtnClick()
{

}

void OptRecord::nextPageBtnClick()
{

}

void OptRecord::closeBtnClick()
{
    this->close();
}

void OptRecord::getModelData()
{
    QList<OptRecordData> recordDataList = MyDataBase::getInstance()->queAllOptRecordData();

    for (int i = 0; i < recordDataList.size(); i++) {
        m_optModel->setItem(i, 0, new QStandardItem(QString(" %1 ").arg(recordDataList[i].recordId)));
        m_optModel->setItem(i, 1, new QStandardItem(QString(" %1 ").arg(recordDataList[i].time)));
        m_optModel->setItem(i, 2, new QStandardItem(QString(" %1 ").arg(recordDataList[i].optorName)));
        m_optModel->setItem(i, 3, new QStandardItem(QString(" %1 ").arg(recordDataList[i].optLog)));
    }
}
