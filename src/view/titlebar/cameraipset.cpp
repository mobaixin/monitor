#include <QDebug>

#include "cameraipset.h"

CameraIpSet::CameraIpSet(QWidget *parent)
    : QDialog(parent)
{
    // 初始化组件
    setWidgetUi();

    // 设置组件样式
    setWidgetStyle();

    // 设置初始数据
    setData();
}

void CameraIpSet::setWidgetUi()
{
    m_ipTableView  = new QTableView(this);
    m_ipModel      = new QStandardItemModel(this);

    m_freshBtn     = new QPushButton(this);
    m_changeIpBtn  = new QPushButton(this);
    m_autoGetIpBtn = new QPushButton(this);
    m_confirmBtn   = new QPushButton(this);
    m_cancelBtn    = new QPushButton(this);

    m_mainLayout  = new QHBoxLayout(this);
    m_rightLayout = new QVBoxLayout();

    m_rightLayout->addWidget(m_freshBtn);
    m_rightLayout->addWidget(m_changeIpBtn);
    m_rightLayout->addWidget(m_autoGetIpBtn);
    m_rightLayout->addWidget(m_confirmBtn);
    m_rightLayout->addWidget(m_cancelBtn);
    m_rightLayout->setContentsMargins(0, 0, 0, 0);
    m_rightLayout->setSpacing(5);

    m_mainLayout->addWidget(m_ipTableView);
    m_mainLayout->addLayout(m_rightLayout);
    m_mainLayout->setContentsMargins(5, 5, 5, 5);
    m_mainLayout->setSpacing(5);

    this->setLayout(m_mainLayout);

    connect(m_freshBtn,     &QPushButton::clicked, this, &CameraIpSet::freshBtnClick);
    connect(m_changeIpBtn,  &QPushButton::clicked, this, &CameraIpSet::changeIpBtnClick);
    connect(m_autoGetIpBtn, &QPushButton::clicked, this, &CameraIpSet::autoGetIpBtnClick);
    connect(m_confirmBtn,   &QPushButton::clicked, this, &CameraIpSet::confirmBtnClick);
    connect(m_cancelBtn,    &QPushButton::clicked, this, &CameraIpSet::cancelBtnClick);

}

void CameraIpSet::setWidgetStyle()
{
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    this->setFixedSize(700, 220);
    this->setWindowTitle("相机IP配置");

    m_ipTableView->setFixedSize(570, this->height());
    m_freshBtn->setFixedSize(100, 40);
    m_changeIpBtn->setFixedSize(100, 40);
    m_autoGetIpBtn->setFixedSize(100, 40);
    m_confirmBtn->setFixedSize(100, 40);
    m_cancelBtn->setFixedSize(100, 40);

    QFont viewFont = this->font();
    viewFont.setPixelSize(15);
    m_ipTableView->setFont(viewFont);
    m_ipTableView->setAlternatingRowColors(true);

    m_ipModel->setHorizontalHeaderLabels({"系列号", "自定义名称", "接口", "状态", "IP地址"});
    m_ipTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_ipTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_ipTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ipTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ipTableView->setModel(m_ipModel);

    m_freshBtn->setFocusPolicy(Qt::NoFocus);
    m_changeIpBtn->setFocusPolicy(Qt::NoFocus);
    m_autoGetIpBtn->setFocusPolicy(Qt::NoFocus);
    m_confirmBtn->setFocusPolicy(Qt::NoFocus);
    m_cancelBtn->setFocusPolicy(Qt::NoFocus);

    m_freshBtn->setFont(viewFont);
    m_changeIpBtn->setFont(viewFont);
    m_autoGetIpBtn->setFont(viewFont);
    m_confirmBtn->setFont(viewFont);
    m_cancelBtn->setFont(viewFont);

    m_freshBtn->setText("刷新");
    m_changeIpBtn->setText("修改IP");
    m_autoGetIpBtn->setText("自定义分配IP");
    m_confirmBtn->setText("确认");
    m_cancelBtn->setText("取消");

}

void CameraIpSet::setData()
{
    getModelData();
    m_ipTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

}

void CameraIpSet::freshBtnClick()
{

}

void CameraIpSet::changeIpBtnClick()
{

}

void CameraIpSet::autoGetIpBtnClick()
{

}

void CameraIpSet::confirmBtnClick()
{

}

void CameraIpSet::cancelBtnClick()
{
    this->close();
}

void CameraIpSet::getModelData()
{
    for (int i = 0; i < 10; i++) {
        m_ipModel->setItem(i, 0, new QStandardItem(QString(" DSGP20603090%1 ").arg(i)));
        m_ipModel->setItem(i, 1, new QStandardItem(QString(" GP20603090%1 ").arg(i)));
        m_ipModel->setItem(i, 2, new QStandardItem(QString(" 192.168.1.9%1 ").arg(i)));
        m_ipModel->setItem(i, 3, new QStandardItem(QString(" 可用  ")));
        m_ipModel->setItem(i, 4, new QStandardItem(QString(" 192.168.1.4%1 ").arg(i)));
    }
}
