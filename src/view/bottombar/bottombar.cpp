﻿#include <QDebug>

#include "src/view/bottombar/bottombar.h"
#include "src/view/imgarea/imgarea.h"
#include "src/view/mainwindow.h"
#include "src/view/common/mysettings.h"

#if _MSC_VER >=1600    // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif

BottomBar *BottomBar::getInstance(QWidget *parent)
{
    static BottomBar *instance = nullptr;
    if (nullptr == instance) {
        instance = new BottomBar(parent);
    }
    return instance;
}

BottomBar::BottomBar(QWidget *parent)
    : QWidget(parent)
{
    // 初始化组件
    setWidgetUi();

    // 设置组件样式
    setWidgetStyle();

    // 设置初始数据
    setData();
}

BottomBar::~BottomBar()
{

}

// 初始化组件
void BottomBar::setWidgetUi()
{
    // 设置底边栏高度
    this->setFixedHeight(80);

    // 初始化组件
    m_pAccLab = new QLabel(this);
    m_pPixLab = new QLabel(this);

    m_pAccMySlider = new MySlider(this);
    m_pPixMySlider = new MySlider(this);

//    m_pAccLine = new QLineEdit(this);
//    m_pPixLine = new QLineEdit(this);

//    m_pAccAddBtn = new QPushButton(this);
//    m_pAccSubBtn = new QPushButton(this);
//    m_pPixAddBtn = new QPushButton(this);
//    m_pPixSubBtn = new QPushButton(this);

//    m_pAccSlider = new QSlider(this);
//    m_pPixSlider = new QSlider(this);

    m_pCurvBtn = new QPushButton(this);
    m_pPolyBtn = new QPushButton(this);
    m_pRectBtn = new QPushButton(this);
    m_pCircleBtn  = new QPushButton(this);
    m_pConCirBtn = new QPushButton(this);
    m_pMaskBtn = new QPushButton(this);
    m_pCopyBtn = new QPushButton(this);
    m_pEraseBtn = new QPushButton(this);
    m_pClearBtn = new QPushButton(this);
    m_pPositionBtn = new QPushButton(this);

    // 初始化布局
    m_pBottomLayout = new QHBoxLayout(this);
    m_pLeftWidLayout = new QGridLayout();
    m_pRighBtnLayout = new QHBoxLayout();

    // 获取场景
//    m_pAreaScene = ImgArea::getInstance()->getScene();

    // 对组件进行布局
    // 左部区域
    m_pLeftWidLayout->addWidget(m_pAccLab,    0, 0, 1, 1);
//    m_pLeftWidLayout->addWidget(m_pAccLine,   0, 1, 1, 1);
//    m_pLeftWidLayout->addWidget(m_pAccSubBtn, 0, 2, 1, 1);
//    m_pLeftWidLayout->addWidget(m_pAccSlider, 0, 3, 1, 1);
//    m_pLeftWidLayout->addWidget(m_pAccAddBtn, 0, 4, 1, 1);
    m_pLeftWidLayout->addWidget(m_pAccMySlider, 0, 1, 1, 4);

    m_pLeftWidLayout->addWidget(m_pPixLab,    1, 0, 1, 1);
//    m_pLeftWidLayout->addWidget(m_pPixLine,   1, 1, 1, 1);
//    m_pLeftWidLayout->addWidget(m_pPixSubBtn, 1, 2, 1, 1);
//    m_pLeftWidLayout->addWidget(m_pPixSlider, 1, 3, 1, 1);
//    m_pLeftWidLayout->addWidget(m_pPixAddBtn, 1, 4, 1, 1);
    m_pLeftWidLayout->addWidget(m_pPixMySlider, 1, 1, 1, 4);

    m_pLeftWidLayout->setContentsMargins(0, 0, 0, 0);
    m_pLeftWidLayout->setSpacing(5);

//    m_leftWid = new QWidget(this);
//    m_leftWid->setLayout(m_pLeftWidLayout);
//    m_leftWid->setAutoFillBackground()

    // 右部区域
    m_pRighBtnLayout->addWidget(m_pCurvBtn);
    m_pRighBtnLayout->addWidget(m_pPolyBtn);
    m_pRighBtnLayout->addWidget(m_pRectBtn);
    m_pRighBtnLayout->addWidget(m_pCircleBtn);
    m_pRighBtnLayout->addWidget(m_pConCirBtn);
    m_pRighBtnLayout->addWidget(m_pMaskBtn);
    m_pRighBtnLayout->addWidget(m_pCopyBtn);
    m_pRighBtnLayout->addWidget(m_pEraseBtn);
    m_pRighBtnLayout->addWidget(m_pClearBtn);
    m_pRighBtnLayout->addWidget(m_pPositionBtn);
    m_pRighBtnLayout->setSpacing(10);

    // 整体布局
    m_pBottomLayout->addLayout(m_pLeftWidLayout);
    m_pBottomLayout->addLayout(m_pRighBtnLayout);
    m_pBottomLayout->addStretch();
    m_pBottomLayout->setContentsMargins(10, 0, 300, 0);

    this->setLayout(m_pBottomLayout);

    // 设置滑动条属性
    m_pAccStep = 1;
    m_pPixStep = 1;

    m_pAccMySlider->setValueRange(0, 255);
    m_pAccMySlider->setStep(m_pAccStep);

    m_pPixMySlider->setValueRange(0, 1000);
    m_pPixMySlider->setStep(m_pPixStep);

//    m_pAccSlider->setOrientation(Qt::Horizontal);
//    m_pAccSlider->setMinimum(0);
//    m_pAccSlider->setMaximum(255);
//    m_pAccSlider->setSingleStep(m_pAccStep);
//    m_pAccSlider->setTracking(true);

//    m_pPixSlider->setOrientation(Qt::Horizontal);
//    m_pPixSlider->setMinimum(0);
//    m_pPixSlider->setMaximum(1000);
//    m_pPixSlider->setSingleStep(m_pPixStep);
//    m_pPixSlider->setTracking(true);

//    m_pAccLine->setText(QString::number(m_pAccSlider->value()));
//    m_pPixLine->setText(QString::number(m_pPixSlider->value()));

//    //启用长按
//    m_pAccAddBtn->setAutoRepeat(true);
//    m_pAccAddBtn->setAutoRepeatDelay(400);
//    m_pAccAddBtn->setAutoRepeatInterval(20);
//    m_pAccSubBtn->setAutoRepeat(true);
//    m_pAccSubBtn->setAutoRepeatDelay(400);
//    m_pAccSubBtn->setAutoRepeatInterval(20);
//    m_pPixAddBtn->setAutoRepeat(true);
//    m_pPixAddBtn->setAutoRepeatDelay(400);
//    m_pPixAddBtn->setAutoRepeatInterval(20);
//    m_pPixSubBtn->setAutoRepeat(true);
//    m_pPixSubBtn->setAutoRepeatDelay(400);
//    m_pPixSubBtn->setAutoRepeatInterval(20);

//    connect(m_pAccAddBtn, &QPushButton::clicked, this, &BottomBar::accAddBtnClick);
//    connect(m_pAccSubBtn, &QPushButton::clicked, this, &BottomBar::accSubBtnClick);
//    connect(m_pPixAddBtn, &QPushButton::clicked, this, &BottomBar::pixAddBtnClick);
//    connect(m_pPixSubBtn, &QPushButton::clicked, this, &BottomBar::pixSubBtnClick);

//    connect(m_pAccSlider, &QSlider::valueChanged, this, &BottomBar::setAccLineValue);
//    connect(m_pPixSlider, &QSlider::valueChanged, this, &BottomBar::setPixLineValue);

//    connect(m_pAccLine, &QLineEdit::editingFinished, this, &BottomBar::setAccSliderValue);
//    connect(m_pPixLine, &QLineEdit::editingFinished, this, &BottomBar::setPixSliderValue);

    connect(m_pCurvBtn,   &QPushButton::clicked, this, &BottomBar::curveBtnClick);
    connect(m_pPolyBtn,   &QPushButton::clicked, this, &BottomBar::polygonBtnClick);
    connect(m_pRectBtn,   &QPushButton::clicked, this, &BottomBar::rectBtnClick);
    connect(m_pCircleBtn, &QPushButton::clicked, this, &BottomBar::circleBtnClick);
    connect(m_pConCirBtn, &QPushButton::clicked, this, &BottomBar::conCircleBtnClick);
    connect(m_pMaskBtn,   &QPushButton::clicked, this, &BottomBar::maskBtnClick);
    connect(m_pCopyBtn,   &QPushButton::clicked, this, &BottomBar::copyBtnClick);
    connect(m_pPositionBtn, &QPushButton::clicked, this, &BottomBar::positionBtnClick);

}

// 设置组件样式
void BottomBar::setWidgetStyle()
{
    this->setAttribute(Qt::WA_TranslucentBackground, true);

//    this->setStyleSheet("background:transparent;");
//    this->setGeometry(0, MainWindow::getInstance()->height() - this->height(), this->width(), this->height());

    m_pAccLab->setFixedSize(60, 20);
    m_pPixLab->setFixedSize(60, 20);

//    m_pAccLine->setFixedSize(50, 20);
//    m_pPixLine->setFixedSize(50, 20);

//    m_pAccAddBtn->setFixedSize(30, 30);
//    m_pAccSubBtn->setFixedSize(30, 30);
//    m_pPixAddBtn->setFixedSize(30, 30);
//    m_pPixSubBtn->setFixedSize(30, 30);

//    m_pAccSlider->setFixedSize(200, 20);
//    m_pPixSlider->setFixedSize(200, 20);

    m_pCurvBtn->setFixedSize(60, 30);
    m_pPolyBtn->setFixedSize(60, 30);
    m_pRectBtn->setFixedSize(60, 30);
    m_pCircleBtn->setFixedSize(60, 30);
    m_pConCirBtn->setFixedSize(60, 30);
    m_pMaskBtn->setFixedSize(60, 30);
    m_pCopyBtn->setFixedSize(60, 30);
    m_pEraseBtn->setFixedSize(60, 30);
    m_pClearBtn->setFixedSize(60, 30);
    m_pPositionBtn->setFixedSize(60, 30);

    m_pAccLab->setText(" 检测精度:");
    m_pPixLab->setText(" 像素:");

//    m_pAccLab->setAutoFillBackground(true);
//    m_pAccLab->setBackgroundRole(QPalette::Window);
//    m_pPixLab->setAutoFillBackground(true);
//    m_pPixLab->setBackgroundRole(QPalette::Window);

    m_pAccLab->setStyleSheet("border-radius:2px;background-color:#F2F2F2;");
    m_pPixLab->setStyleSheet("border-radius:2px;background-color:#F2F2F2;");

    m_pAccMySlider->setValue(30);
    m_pPixMySlider->setValue(300);

    m_pCurvBtn->setText("曲线");
    m_pPolyBtn->setText("多边形");
    m_pRectBtn->setText("矩形");
    m_pCircleBtn->setText("圆形");
    m_pConCirBtn->setText("环形");
    m_pMaskBtn->setText("屏蔽区");
    m_pCopyBtn->setText("复制");
    m_pEraseBtn->setText("擦除");
    m_pClearBtn->setText("清除");
    m_pPositionBtn->setText("↑↓");

    m_pCurvBtn->setCheckable(true);
    m_pPolyBtn->setCheckable(true);
    m_pMaskBtn->setCheckable(true);

    QString spBtnStyleStr = "QPushButton{background:#00BFFF;color:#000000;border-radius:5px;font-size:14px;border: 1px groove #F3F781;}"
                            "QPushButton:hover{background:#58D3F7;color:#000000;}"
                            "QPushButton:pressed{background:#086A87;color:#FAFAFA;}"
                            "QPushButton:checked{background:#086A87;color:#FAFAFA;}";

    QString btnStyleStr = "QPushButton{background:#00BFFF;color:#000000;border-radius:5px;font-size:14px;border: 1px groove #F3F781;}"
                          "QPushButton:hover{background:#58D3F7;color:#000000;}"
                          "QPushButton:pressed{background:#00BFFF;color:#000000;}";

    m_pCurvBtn->setStyleSheet(spBtnStyleStr);
    m_pPolyBtn->setStyleSheet(spBtnStyleStr);
    m_pRectBtn->setStyleSheet(btnStyleStr);
    m_pCircleBtn->setStyleSheet(btnStyleStr);
    m_pConCirBtn->setStyleSheet(btnStyleStr);
    m_pMaskBtn->setStyleSheet(spBtnStyleStr);
    m_pCopyBtn->setStyleSheet(btnStyleStr);
    m_pEraseBtn->setStyleSheet(btnStyleStr);
    m_pClearBtn->setStyleSheet(btnStyleStr);
    m_pPositionBtn->setStyleSheet(btnStyleStr);

    connect(m_pAccMySlider, &MySlider::valueChange, this, &BottomBar::updateItemAcc);
    connect(m_pPixMySlider, &MySlider::valueChange, this, &BottomBar::updateItemPix);
}

void BottomBar::setData()
{
    m_isCreatePolygon = false;
    m_isCreateCurve   = false;
    m_isCreateMask = false;

    m_isUpdateAcc = false;
    m_isUpdatePix = false;
}

void BottomBar::setClearBtnConnect()
{
    connect(m_pEraseBtn, &QPushButton::clicked, ImgArea::getInstance(), &ImgArea::eraseShape);
    connect(m_pClearBtn, &QPushButton::clicked, ImgArea::getInstance(), &ImgArea::clearShapes);
}

void BottomBar::setBtnEnabled(bool enable)
{
    m_pClearBtn->setEnabled(enable);
}

void BottomBar::setAccuracy(int acc)
{
    m_isUpdateAcc = false;
    m_pAccMySlider->setValue(acc);
}

int BottomBar::getAccuracy()
{
    int acc = m_pAccMySlider->getEditValue().toInt();

    return acc;
}

void BottomBar::setPixel(int pix)
{
    m_isUpdatePix = false;
    m_pPixMySlider->setValue(pix);
}

int BottomBar::getPixel()
{
    int pix = m_pPixMySlider->getEditValue().toInt();

    return pix;
}

void BottomBar::createRect(QPointF point)
{
    m_newMyRect = new MyRectangle(point.x(), point.y(), 5, 5, MyGraphicsItem::ItemType::Rectangle);
//    m_pAreaScene->addItem(m_newMyRect);
    ImgArea::getInstance()->addShapeItemToList(m_newMyRect);

    m_newMyRect->setAccuracy(getAccuracy());
    m_newMyRect->setPixel(getPixel());
}

MyRectangle *BottomBar::getNewMyRect()
{
    return m_newMyRect;
}

void BottomBar::createCircle(QPointF point)
{
    m_newMyCircle = new MyCircle(point.x(), point.y(), 5, MyGraphicsItem::ItemType::Circle);
//    m_pAreaScene->addItem(m_newMyCircle);
    ImgArea::getInstance()->addShapeItemToList(m_newMyCircle);

    m_newMyCircle->setAccuracy(getAccuracy());
    m_newMyCircle->setPixel(getPixel());
}

MyCircle *BottomBar::getNewMyCircle()
{
    return m_newMyCircle;
}

void BottomBar::createConCircle(QPointF point)
{
    m_newMyConCircle = new MyConcentricCircle(point.x(), point.y(), 10, 30, MyGraphicsItem::ItemType::Concentric_Circle);
//    m_pAreaScene->addItem(m_newMyConCircle);
    ImgArea::getInstance()->addShapeItemToList(m_newMyConCircle);

    m_newMyConCircle->setAccuracy(getAccuracy());
    m_newMyConCircle->setPixel(getPixel());
}

MyConcentricCircle *BottomBar::getNewMyConCircle()
{
    return m_newMyConCircle;
}

void BottomBar::updateCreateCurve()
{
    m_pCurvBtn->click();
}

void BottomBar::updateCreatePolygon()
{
    if (m_isCreatePolygon) {
        m_pPolyBtn->click();
    } else if (m_isCreateMask) {
        m_pMaskBtn->click();
    }

}

//void BottomBar::accAddBtnClick()
//{
//    m_pAccSlider->setValue(m_pAccSlider->value() + m_pAccStep);
//}

//void BottomBar::accSubBtnClick()
//{
//    m_pAccSlider->setValue(m_pAccSlider->value() - m_pAccStep);
//}

//void BottomBar::pixAddBtnClick()
//{
//    m_pPixSlider->setValue(m_pPixSlider->value() + m_pPixStep);
//}

//void BottomBar::pixSubBtnClick()
//{
//    m_pPixSlider->setValue(m_pPixSlider->value() - m_pPixStep);
//}

//void BottomBar::setAccLineValue(int value)
//{
//    m_pAccLine->setText(QString::number(value));
//}

//void BottomBar::setPixLineValue(int value)
//{
//    m_pPixLine->setText(QString::number(value));
//}

//void BottomBar::setAccSliderValue()
//{
//    int value = m_pAccLine->text().toInt();
//    m_pAccSlider->setValue(value);
//}

//void BottomBar::setPixSliderValue()
//{
//    int value = m_pPixLine->text().toInt();
//    m_pPixSlider->setValue(value);
//}

void BottomBar::circleBtnClick()
{
//    MyCircle *circle = new MyCircle(500, 300, 50, MyGraphicsItem::Circle);
//    m_pAreaScene->addItem(circle);

//    circle->setAccuracy(getAccuracy());
//    circle->setPixel(getPixel());

    ImgArea::getInstance()->getScene()->startCreateCircle();

    OptRecord::addOptRecord("点击圆形");
}

void BottomBar::conCircleBtnClick()
{
//    MyConcentricCircle *conCircle = new MyConcentricCircle(500, 300, 50, 80, MyGraphicsItem::ItemType::Concentric_Circle);
//    m_pAreaScene->addItem(conCircle);

//    conCircle->setAccuracy(getAccuracy());
//    conCircle->setPixel(getPixel());

    ImgArea::getInstance()->getScene()->startCreateConCircle();

    OptRecord::addOptRecord("点击环形");
}

void BottomBar::rectBtnClick()
{
//    MyRectangle *rectangle = new MyRectangle(500, 300, 100, 50, MyGraphicsItem::ItemType::Rectangle);
//    m_pAreaScene->addItem(rectangle);

//    rectangle->setAccuracy(getAccuracy());
//    rectangle->setPixel(getPixel());

    ImgArea::getInstance()->getScene()->startCreateRect();

    OptRecord::addOptRecord("点击矩形");
}

void BottomBar::polygonBtnClick()
{
    if (!m_isCreatePolygon) {
        ImgArea::getInstance()->getScene()->startCreatePolygon();
        m_isCreatePolygon = true;

        m_pPolyBtn->setText("完成绘制");

        m_newPolygon = new MyPolygon(MyGraphicsItem::ItemType::Polygon);
//        m_pAreaScene->addItem(polygon);
        ImgArea::getInstance()->addShapeItemToList(m_newPolygon);

        m_newPolygon->setAccuracy(getAccuracy());
        m_newPolygon->setPixel(getPixel());

        connect(ImgArea::getInstance()->getScene(), &MyGraphicsScene::updatePolyPoint, m_newPolygon, &MyPolygon::pushPoint);
//        connect(m_pAreaScene, &MyGraphicsScene::createFinished, polygon, )

        OptRecord::addOptRecord("点击多边形");
    } else {
        m_pPolyBtn->setText("多边形");
        m_isCreatePolygon = false;

        bool state = ImgArea::getInstance()->judgePolygonState(m_newPolygon);
        ImgArea::getInstance()->getScene()->finishCreatePloygon();

        OptRecord::addOptRecord("点击完成绘制");
    }
}

void BottomBar::curveBtnClick()
{
    if (!m_isCreateCurve) {
        ImgArea::getInstance()->getScene()->startCreateCurve();
        m_isCreateCurve = true;

        m_pCurvBtn->setText("完成绘制");

        MyCurve *curve = new MyCurve(MyGraphicsItem::ItemType::Curve);
//        m_pAreaScene->addItem(curve);
        ImgArea::getInstance()->addShapeItemToList(curve);

        curve->setAccuracy(getAccuracy());
        curve->setPixel(getPixel());

        connect(ImgArea::getInstance()->getScene(), &MyGraphicsScene::updateCurvePoint, curve, &MyCurve::pushPoint);
//        connect(m_pAreaScene, &MyGraphicsScene::createFinished, polygon, )

        OptRecord::addOptRecord("点击曲线");

    } else {
        m_pCurvBtn->setText("曲线");
        m_isCreateCurve = false;

        ImgArea::getInstance()->getScene()->finishCreateCurve();

        OptRecord::addOptRecord("点击完成绘制");
    }
}

void BottomBar::maskBtnClick()
{
    if (!m_isCreateMask) {
        ImgArea::getInstance()->getScene()->startCreatePolygon();
        m_isCreateMask = true;

        m_pMaskBtn->setText("完成绘制");

        MyPolygon *polygon = new MyPolygon(MyGraphicsItem::ItemType::Polygon_Mask);
        polygon->setMask(true);
//        m_pAreaScene->addItem(polygon);
        ImgArea::getInstance()->addShapeItemToList(polygon);

        polygon->setAccuracy(-1);
        polygon->setPixel(-1);
        connect(ImgArea::getInstance()->getScene(), &MyGraphicsScene::updatePolyPoint, polygon, &MyPolygon::pushPoint);
//        connect(m_pAreaScene, &MyGraphicsScene::createFinished, polygon, )

        OptRecord::addOptRecord("点击屏蔽区");
    } else {
        m_pMaskBtn->setText("屏蔽区");
        m_isCreateMask = false;

        ImgArea::getInstance()->getScene()->finishCreatePloygon();

        OptRecord::addOptRecord("点击完成绘制");
    }
}

void BottomBar::copyBtnClick()
{
    OptRecord::addOptRecord("点击复制");

    ImgArea::getInstance()->copySelectedShapeItem();
}

void BottomBar::positionBtnClick()
{
    if (this->y() > 300) {
        this->setGeometry(0, 50, this->width(), this->height());
        MainWindow::getInstance()->adjustBottomBarPos();
    } else {
        this->setGeometry(0, MainWindow::getInstance()->height() - this->height(), this->width(), this->height());
    }
}

void BottomBar::updateItemAcc(int acc)
{
    if (!m_isUpdateAcc) {
        m_isUpdateAcc = true;
        return ;
    }

    QList<QGraphicsItem *> selectItemList = ImgArea::getInstance()->getSelectItemList();
    if (!selectItemList.isEmpty()) {
        QGraphicsItem *temp = selectItemList.first();
        MyGraphicsItem *item = static_cast<MyGraphicsItem *>(temp);

        if (item->getAccuracy() != -1) {
            item->setAccuracy(acc);
            item->update();
        }
    }

    MySettings::getInstance()->setValue(ShapeSection, AccuracyKey, QString::number(acc));
}

void BottomBar::updateItemPix(int pix)
{
    if (!m_isUpdatePix) {
        m_isUpdatePix = true;
        return ;
    }

    QList<QGraphicsItem *> selectItemList = ImgArea::getInstance()->getSelectItemList();
    if (!selectItemList.isEmpty()) {
        QGraphicsItem *temp = selectItemList.first();
        MyGraphicsItem *item = static_cast<MyGraphicsItem *>(temp);

        if (item->getPixel() != -1) {
            item->setPixel(pix);
            item->update();
        }
    }

    MySettings::getInstance()->setValue(ShapeSection, PixelKey, QString::number(pix));
}

