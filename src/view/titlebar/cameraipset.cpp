#include <QDebug>

#include "cameraipset.h"
#include "src/view/titlebar/optrecord.h"

#if _MSC_VER >=1600    // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif

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
    m_ipTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
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
    m_ipTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
}

void CameraIpSet::freshBtnClick()
{
    OptRecord::addOptRecord("点击刷新IP");

}

void CameraIpSet::changeIpBtnClick()
{
    OptRecord::addOptRecord("点击修改IP");

    m_myIpInputDialog = new MyIpInputDialog();
    m_myIpInputDialog->exec();
}

void CameraIpSet::autoGetIpBtnClick()
{
    OptRecord::addOptRecord("点击自动分配IP");

}

void CameraIpSet::confirmBtnClick()
{
    OptRecord::addOptRecord("点击确认");

}

void CameraIpSet::cancelBtnClick()
{
    OptRecord::addOptRecord("点击取消");
    this->close();
}

void CameraIpSet::getModelData()
{
//    for (int i = 0; i < 10; i++) {
//        m_ipModel->setItem(i, 0, new QStandardItem(QString(" DSGP20603090%1 ").arg(i)));
//        m_ipModel->setItem(i, 1, new QStandardItem(QString(" GP20603090%1 ").arg(i)));
//        m_ipModel->setItem(i, 2, new QStandardItem(QString(" 192.168.1.9%1 ").arg(i)));
//        m_ipModel->setItem(i, 3, new QStandardItem(QString(" 可用  ")));
//        m_ipModel->setItem(i, 4, new QStandardItem(QString(" 192.168.1.4%1 ").arg(i)));
//    }

    m_cameraIPDataList = MyDataBase::getInstance()->queAllCameraIPData();

    for (int i = 0; i < m_cameraIPDataList.size(); i++) {
        m_ipModel->setItem(i, 0, new QStandardItem(QString(" %1 ").arg(m_cameraIPDataList[i].serialId)));
        m_ipModel->setItem(i, 1, new QStandardItem(QString(" %1 ").arg(m_cameraIPDataList[i].nickName)));
        m_ipModel->setItem(i, 2, new QStandardItem(QString(" %1 ").arg(m_cameraIPDataList[i].portIp)));
        m_ipModel->setItem(i, 3, new QStandardItem(QString(" %1 ").arg(m_cameraIPDataList[i].state)));
        m_ipModel->setItem(i, 4, new QStandardItem(QString(" %1 ").arg(m_cameraIPDataList[i].cameraIp)));
    }
}

MyIpInputDialog::MyIpInputDialog(QWidget *parent)
    : QDialog(parent)
{
    // 初始化组件
    setWidgetUi();

    // 设置组件样式
    setWidgetStyle();

    // 设置初始数据
    setData();
}

void MyIpInputDialog::setWidgetUi()
{
    // 初始化组件
    m_captionLab  = new QLabel(this);
    m_ipInputEdit = new QLineEdit(this);
    m_cancelBtn   = new QPushButton(this);
    m_confirmBtn  = new QPushButton(this);

    m_btnLayout  = new QHBoxLayout();
    m_mainLayout = new QVBoxLayout(this);

    // 组件布局
    m_btnLayout->addWidget(m_cancelBtn);
    m_btnLayout->addWidget(m_confirmBtn);
    m_btnLayout->setContentsMargins(0, 0, 0, 0);
    m_btnLayout->setSpacing(5);

    m_mainLayout->addWidget(m_captionLab);
    m_mainLayout->addWidget(m_ipInputEdit);
    m_mainLayout->addLayout(m_btnLayout);
    m_mainLayout->setContentsMargins(5, 5, 5, 5);
    m_mainLayout->setSpacing(10);

    this->setLayout(m_mainLayout);

    connect(m_cancelBtn,  &QPushButton::clicked, this, &MyIpInputDialog::cancelBtnClick);
    connect(m_confirmBtn, &QPushButton::clicked, this, &MyIpInputDialog::confirmBtnClick);

}

void MyIpInputDialog::setWidgetStyle()
{
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    this->setModal(true);
    this->setFixedSize(300, 110);
    this->setWindowTitle("修改IP");

    m_captionLab->setFixedHeight(20);
    m_ipInputEdit->setFixedHeight(30);
    m_cancelBtn->setFixedHeight(30);
    m_confirmBtn->setFixedHeight(30);

    m_captionLab->setText("请输入IP地址:");
    m_cancelBtn->setText("取消");
    m_confirmBtn->setText("确定");

    QFont viewFont = this->font();
    viewFont.setPixelSize(15);

    m_captionLab->setFont(viewFont);
    m_ipInputEdit->setFont(viewFont);
    m_cancelBtn->setFont(viewFont);
    m_confirmBtn->setFont(viewFont);

    m_cancelBtn->setFocusPolicy(Qt::NoFocus);
//    m_confirmBtn->setFocusPolicy(Qt::NoFocus);

}

void MyIpInputDialog::setData()
{

}

void MyIpInputDialog::cancelBtnClick()
{
    this->close();
}

void MyIpInputDialog::confirmBtnClick()
{
    emit getValues(m_ipInputEdit->text());

    this->close();
}
