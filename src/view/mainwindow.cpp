#include <QDesktopWidget>
#include <QApplication>
#include <QScreen>
#include <QDebug>

#include "mainwindow.h"

MainWindow *MainWindow::getInstance()
{
    static MainWindow *instance = nullptr;
    if (nullptr == instance) {
        instance = new MainWindow();
    }
    return instance;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 初始化组件
    setWidgetUi();

    // 设置组件样式
    setWidgetStyle();

    // 设置初始数据
    setData();
}

MainWindow::~MainWindow()
{
    m_pImgArea->deleteLater();
}

// 初始化组件
void MainWindow::setWidgetUi()
{
    MyDataBase::getInstance();

    m_pMainWid   = new QWidget(this);
    m_pTitleBar  = TitleBar::getInstance(this);
    m_pImgArea   = ImgArea::getInstance(this);
    m_pBottomBar = BottomBar::getInstance(this);
    m_pSideBar   = SideBar::getInstance(this);

    m_pMainLayout = new QVBoxLayout(m_pMainWid);

    // 组件布局
    m_pMainLayout->addWidget(m_pTitleBar);
    m_pMainLayout->addWidget(m_pImgArea);
    m_pMainLayout->setContentsMargins(0, 0, 0, 0);
    m_pMainLayout->setSpacing(0);

    m_pMainWid->setLayout(m_pMainLayout);

    this->setCentralWidget(m_pMainWid);

    // 设置标题文字
    m_pTitleBar->setTitleLab("相机1");
    m_pTitleBar->setFixedHeight(30);

    m_pImgArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

}

// 设置组件样式
void MainWindow::setWidgetStyle()
{
    this->setWindowFlags(Qt::Window);
    this->setFixedSize(1350, 750);
//    this->showFullScreen();
//    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowTitle("模具监视器");

    QRect availableGeometry = qApp->primaryScreen()->availableGeometry();
    this->move((availableGeometry.width() - this->width())/2,
               (availableGeometry.height() - this->height())/2);

    m_pBottomBar->setGeometry(0, this->height() - m_pBottomBar->height(),
                              this->width(), this->height());
    m_pBottomBar->show();

    m_pSideBar->setGeometry(this->width() - m_pSideBar->width(), m_pTitleBar->height(),
                            this->width(), m_pImgArea->height());
    m_pSideBar->show();


    m_pImgArea->setRunState(true);
    m_pImgArea->setFixedSize(this->width(), this->height() - m_pTitleBar->height());
    m_pImgArea->setSceneSize();

    showMonitorSet(false);
}

void MainWindow::setData()
{
    // 设置图片
//    m_pImgArea->loadImage("D:/image/page01.png");
//    m_pImgArea->loadImage(":/img/page01.png");
}

void MainWindow::closeEvent(QCloseEvent *)
{
    m_pImgArea->deleteLater();
}

void MainWindow::setRunState(bool isStart)
{
    m_pImgArea->setRunState(isStart);
}

void MainWindow::showMonitorSet(bool isDisplay)
{
    if (isDisplay) {
        m_pBottomBar->show();
    } else {
        m_pBottomBar->hide();
    }
    m_pSideBar->setDisplayState(isDisplay);
}

void MainWindow::setDetectObject()
{
    bool isDetectMold = m_pSideBar->getIsDetectMold();
    int  curIdx = m_pSideBar->getCurrentIdx();

    m_pImgArea->setSampleLab(isDetectMold, curIdx);
}

void MainWindow::saveMold()
{
    QGraphicsPixmapItem *imageItem;
    QList<ShapeItemData> shapeItemList;

    imageItem = m_pImgArea->getImageItem();
    shapeItemList = m_pImgArea->getShapeItems();

//    m_pSideBar->saveImageMold(imageItem);
//    m_pSideBar->saveShapeMold(shapeItemList);

//    m_pImgArea->saveAsImage();
}

void MainWindow::loadMold()
{
    QGraphicsPixmapItem *imageItem;
    QList<QGraphicsItem *> shapeItemList;

    imageItem = m_pSideBar->getImageMold();
    shapeItemList = m_pSideBar->getShapeMold();

    m_pImgArea->loadImageItem(imageItem);
    m_pImgArea->loadShapeItem(shapeItemList);
}

void MainWindow::adjustBottomBarPos()
{
    if (m_pBottomBar->y() < 300 && m_pSideBar->x() < 500) {
        m_pBottomBar->setGeometry(m_pSideBar->width(), 50,
                                  m_pBottomBar->width(), m_pBottomBar->height());
    } else if (m_pBottomBar->y() < 300 && m_pSideBar->x() > 500) {
        m_pBottomBar->setGeometry(0, 50,
                                  m_pBottomBar->width(), m_pBottomBar->height());
    }
}


