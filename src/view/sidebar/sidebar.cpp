#include <QDebug>

#include "src/view/sidebar/sidebar.h"
#include "src/view/mainwindow.h"
#include "src/view/imgarea/imgarea.h"

SideBar *SideBar::getInstance(QWidget *parent)
{
    static SideBar *instance = nullptr;
    if (nullptr == instance) {
        instance = new SideBar(parent);
    }
    return instance;
}

SideBar::SideBar(QWidget *parent)
    : QWidget(parent)
{
    // 初始化组件
    setWidgetUi();

    // 设置组件样式
    setWidgetStyle();

    // 设置初始数据
    setData();
}

SideBar::~SideBar()
{

}

// 初始化组件
void SideBar::setWidgetUi()
{
    // 初始化组件
    m_thimbleBox  = new QRadioButton(this);
    m_openMoldBox = new QRadioButton(this);
    m_canThimbleBox   = new QRadioButton(this);
    m_canClampMoldBox = new QRadioButton(this);

    m_positionBtn  = new QPushButton(this);
    m_checkMoldBtn = new QPushButton(this);
    m_productBtn   = new QPushButton(this);

    m_orderLab = new QLabel(this);

    m_saveMoldBtn = new QPushButton(this);
    m_homePageBtn = new QPushButton(this);
    m_prevPageBtn = new QPushButton(this);
    m_nextPageBtn = new QPushButton(this);
    m_delMoldBtn  = new QPushButton(this);
    m_clearMoldBtn = new QPushButton(this);

    m_sideBarLayout = new QVBoxLayout(this);

    m_btnGroup = new QButtonGroup(this);

    // 对组件进行布局
    m_sideBarLayout->addWidget(m_thimbleBox);
    m_sideBarLayout->addWidget(m_openMoldBox);
    m_sideBarLayout->addWidget(m_canThimbleBox);
    m_sideBarLayout->addWidget(m_canClampMoldBox);
    m_sideBarLayout->addWidget(m_positionBtn);
    m_sideBarLayout->addWidget(m_checkMoldBtn);
    m_sideBarLayout->addWidget(m_productBtn);
    m_sideBarLayout->addWidget(m_orderLab);
    m_sideBarLayout->addWidget(m_saveMoldBtn);
    m_sideBarLayout->addWidget(m_homePageBtn);
    m_sideBarLayout->addWidget(m_prevPageBtn);
    m_sideBarLayout->addWidget(m_nextPageBtn);
    m_sideBarLayout->addWidget(m_delMoldBtn);
    m_sideBarLayout->addWidget(m_clearMoldBtn);
    m_sideBarLayout->addStretch();
    m_sideBarLayout->setContentsMargins(0, 30, 0, 0);
    m_sideBarLayout->setSpacing(10);

    this->setLayout(m_sideBarLayout);

    m_thimbleBox->setAutoExclusive(false);
    m_openMoldBox->setAutoExclusive(false);
    m_canThimbleBox->setAutoExclusive(false);
    m_canClampMoldBox->setAutoExclusive(false);

    m_checkMoldBtn->setCheckable(true);
    m_productBtn->setCheckable(true);
    m_checkMoldBtn->setChecked(true);

    m_btnGroup->addButton(m_checkMoldBtn);
    m_btnGroup->addButton(m_productBtn);
    m_btnGroup->setExclusive(true);

    connect(m_positionBtn,  &QPushButton::clicked, this, &SideBar::positionBtnClick);
    connect(m_checkMoldBtn, &QPushButton::clicked, this, &SideBar::checkMoldBtnClick);
    connect(m_productBtn,   &QPushButton::clicked, this, &SideBar::productBtnClick);
    connect(m_saveMoldBtn,  &QPushButton::clicked, this, &SideBar::saveMoldBtnClick);
    connect(m_homePageBtn,  &QPushButton::clicked, this, &SideBar::homePageBtnClick);
    connect(m_prevPageBtn,  &QPushButton::clicked, this, &SideBar::prevPageBtnClick);
    connect(m_nextPageBtn,  &QPushButton::clicked, this, &SideBar::nextPageBtnClick);
    connect(m_delMoldBtn,   &QPushButton::clicked, this, &SideBar::delMoldBtnClick);
    connect(m_clearMoldBtn, &QPushButton::clicked, this, &SideBar::clearMoldBtnClick);

}

// 设置组件样式
void SideBar::setWidgetStyle()
{
    // 设置侧边栏宽度
    this->setFixedSize(120, 600);
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    m_thimbleBox->setFixedSize(100, 20);
    m_openMoldBox->setFixedSize(100, 20);
    m_canThimbleBox->setFixedSize(100, 20);
    m_canClampMoldBox->setFixedSize(100, 20);

    m_positionBtn->setFixedSize(100, 30);
    m_checkMoldBtn->setFixedSize(100, 30);
    m_productBtn->setFixedSize(100, 30);

    m_orderLab->setFixedSize(100, 20);

    m_saveMoldBtn->setFixedSize(100, 30);
    m_homePageBtn->setFixedSize(100, 30);
    m_prevPageBtn->setFixedSize(100, 30);
    m_nextPageBtn->setFixedSize(100, 30);
    m_delMoldBtn->setFixedSize(100, 30);
    m_clearMoldBtn->setFixedSize(100, 30);

    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::blue);//设置颜色

    m_thimbleBox->setPalette(pe);
    m_openMoldBox->setPalette(pe);
    m_canThimbleBox->setPalette(pe);
    m_canClampMoldBox->setPalette(pe);

    m_thimbleBox->setText("  顶针");
    m_openMoldBox->setText("  开模");
    m_canThimbleBox->setText("  可顶针");
    m_canClampMoldBox->setText("  可合模");

    m_positionBtn->setText("↔");
    m_checkMoldBtn->setText("检模");
    m_productBtn->setText("产品");

    m_orderLab->setText("1/1");

    m_saveMoldBtn->setText("保存模板");
    m_homePageBtn->setText("首页");
    m_prevPageBtn->setText("上一页");
    m_nextPageBtn->setText("下一页");
    m_delMoldBtn->setText("删除模板");
    m_clearMoldBtn->setText("清空模板");

    m_canThimbleBox->setStyleSheet("QRadioButton::indicator{background-color:green;width:12px;height:12px;border-radius:6px;}");
    m_canClampMoldBox->setStyleSheet("QRadioButton::indicator{background-color:red;width:12px;height:12px;border-radius:6px;}");

}

// 设置初始数据
void SideBar::setData()
{
    m_isDetectMold = true;

    m_deteMoldNum = 0;
    m_prodMoldNum = 0;

    m_curDeteMoldIdx = 1;
    m_curProdMoldIdx = 1;
}

void SideBar::setDisplayState(bool isDisplay)
{
    if (isDisplay) {
        m_saveMoldBtn->show();
        m_homePageBtn->show();
        m_prevPageBtn->show();
        m_nextPageBtn->show();
        m_delMoldBtn->show();
        m_clearMoldBtn->show();
        m_orderLab->show();
    } else {
        m_saveMoldBtn->hide();
        m_homePageBtn->hide();
        m_prevPageBtn->hide();
        m_nextPageBtn->hide();
        m_delMoldBtn->hide();
        m_clearMoldBtn->hide();
        m_orderLab->hide();
    }
}

bool SideBar::getIsDetectMold()
{
    return m_isDetectMold;
}

int SideBar::getCurrentIdx()
{
    int curIdx = 1;
    if (m_isDetectMold) {
        curIdx = m_curDeteMoldIdx;
    } else {
        curIdx = m_curProdMoldIdx;
    }

    return curIdx;
}

void SideBar::saveImageMold(QGraphicsPixmapItem *imgItem)
{
    if (m_isDetectMold) {
        m_deteImgItemList.append(imgItem);
    } else {
        m_prodImgItemList.append(imgItem);
    }
}

void SideBar::saveShapeMold(QList<QGraphicsItem *> itemList)
{
    if (m_isDetectMold) {
        m_deteShapeItemList.append(itemList);
        m_deteMoldNum += 1;
    } else {
        m_prodShapeItemList.append(itemList);
        m_prodMoldNum += 1;
    }
}

QGraphicsPixmapItem *SideBar::getImageMold()
{
    QGraphicsPixmapItem *imageItem = nullptr;
    if (m_isDetectMold) {
        if (m_curDeteMoldIdx > 0 && m_deteMoldNum > 0) {
            imageItem = m_deteImgItemList.at(m_curDeteMoldIdx - 1);
        }
    } else {
        if (m_curProdMoldIdx > 0 && m_prodMoldNum > 0) {
            imageItem = m_prodImgItemList.at(m_curProdMoldIdx - 1);
        }
    }
    return imageItem;
}

QList<QGraphicsItem *> SideBar::getShapeMold()
{
    QList<QGraphicsItem *> shapeItem;
    if (m_isDetectMold) {
        if (m_curDeteMoldIdx > 0 && m_deteMoldNum > 0) {
            shapeItem = m_deteShapeItemList.at(m_curDeteMoldIdx - 1);
        }
    } else {
        if (m_curProdMoldIdx > 0 && m_prodMoldNum > 0) {
            shapeItem = m_prodShapeItemList.at(m_curProdMoldIdx - 1);
        }
    }
    return shapeItem;
}

void SideBar::positionBtnClick()
{
    if (this->x() > 500) {
        this->setGeometry(10, this->y(), this->width(), this->height());
    } else {
        this->setGeometry(MainWindow::getInstance()->width() - this->width(), this->y(),
                          this->width(), this->height());
    }
    MainWindow::getInstance()->adjustBottomBarPos();
}

void SideBar::checkMoldBtnClick()
{
    m_isDetectMold = true;
    MainWindow::getInstance()->setDetectObject();
    updateOrderLab();
}

void SideBar::productBtnClick()
{
    m_isDetectMold = false;
    MainWindow::getInstance()->setDetectObject();
    updateOrderLab();
}

void SideBar::saveMoldBtnClick()
{
    MainWindow::getInstance()->saveMold();
    updateOrderLab();
}

void SideBar::homePageBtnClick()
{
    if (m_isDetectMold) {
        if (m_deteMoldNum > 0) {
            m_curDeteMoldIdx = 1;
        }
    } else {
        if (m_prodMoldNum > 0) {
            m_curProdMoldIdx = 1;
        }
    }
    MainWindow::getInstance()->setDetectObject();
    updateOrderLab();
}

void SideBar::prevPageBtnClick()
{
    bool isChange = false;
    if (m_isDetectMold) {
        if (m_curDeteMoldIdx - 1 > 0) {
            m_curDeteMoldIdx -= 1;
            isChange = true;
        }
    } else {
        if (m_curProdMoldIdx - 1 > 0) {
            m_curProdMoldIdx -= 1;
            isChange = true;
        }
    }
    if (isChange) {
        updateOrderLab();
        MainWindow::getInstance()->setDetectObject();
//        MainWindow::getInstance()->loadMold();
    }

}

void SideBar::nextPageBtnClick()
{
    bool isChange = false;
    if (m_isDetectMold) {
        if (m_curDeteMoldIdx + 1 <= m_deteMoldNum) {
            m_curDeteMoldIdx += 1;
            isChange = true;
        }
    } else {
        if (m_curProdMoldIdx + 1 <= m_prodMoldNum) {
            m_curProdMoldIdx += 1;
            isChange = true;
        }
    }
    if (isChange) {
        updateOrderLab();
        MainWindow::getInstance()->setDetectObject();
//        MainWindow::getInstance()->loadMold();
    }
}

void SideBar::delMoldBtnClick()
{
    if (m_isDetectMold) {
        if (m_deteMoldNum > 0) {
            m_deteImgItemList.removeAt(m_curDeteMoldIdx - 1);
            m_deteShapeItemList.removeAt(m_curDeteMoldIdx - 1);

            if (m_curDeteMoldIdx == m_deteMoldNum) {
                m_curDeteMoldIdx -= 1;
            }
            m_deteMoldNum -= 1;
        }
    } else {
        if (m_prodMoldNum > 0) {
            m_prodImgItemList.removeAt(m_curProdMoldIdx - 1);
            m_prodShapeItemList.removeAt(m_curProdMoldIdx - 1);

            if (m_curProdMoldIdx == m_prodMoldNum) {
                m_curProdMoldIdx -= 1;
            }
            m_prodMoldNum -= 1;
        }
    }
    updateOrderLab();
    MainWindow::getInstance()->setDetectObject();
}

void SideBar::clearMoldBtnClick()
{
    if (m_isDetectMold) {
        m_deteImgItemList.clear();
        m_deteShapeItemList.clear();

        m_curDeteMoldIdx = 1;
        m_deteMoldNum = 0;
    } else {
        m_prodImgItemList.clear();
        m_prodShapeItemList.clear();

        m_curProdMoldIdx = 1;
        m_prodMoldNum = 0;
    }
    updateOrderLab();
    MainWindow::getInstance()->setDetectObject();
}

void SideBar::updateOrderLab()
{
    QString text;
    if (m_isDetectMold) {
        text = QString("%1/%2").arg(m_curDeteMoldIdx).arg(m_deteMoldNum);
    } else {
        text = QString("%1/%2").arg(m_curProdMoldIdx).arg(m_prodMoldNum);
    }
    m_orderLab->setText(text);
}
