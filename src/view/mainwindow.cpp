#include <QDesktopWidget>
#include <QApplication>
#include <QScreen>
#include <QTimer>
#include <QDebug>

#include "mainwindow.h"
#include "src/serialport/myserialport.h"

#if _MSC_VER >=1600    // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif

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

    MyDataBase::getInstance()->deleteLater();
    MySettings::getInstance()->deleteLater();
    MySerialPort::getInstance()->deleteLater();
}

// 初始化组件
void MainWindow::setWidgetUi()
{
    this->setWindowFlags(Qt::Window);
    this->setFixedSize(1350, 750);
//    this->showFullScreen();

    MyDataBase::getInstance();
    MySettings::getInstance();
    MySerialPort::getInstance();

    m_pMainWid   = new QWidget(this);
    m_pTitleBar  = TitleBar::getInstance(this);
    m_pBottomBar = BottomBar::getInstance(this);
    m_pSideBar   = SideBar::getInstance(this);
    m_pImgArea   = ImgArea::getInstance(this);

    m_pMainLayout = new QVBoxLayout(m_pMainWid);

    // 组件布局
    m_pMainLayout->addWidget(m_pTitleBar);
    m_pMainLayout->addWidget(m_pImgArea);
    m_pMainLayout->setContentsMargins(0, 0, 0, 0);
    m_pMainLayout->setSpacing(0);

    m_pMainWid->setLayout(m_pMainLayout);

    this->setCentralWidget(m_pMainWid);

    // 按钮事件绑定
    m_pBottomBar->setClearBtnConnect();

    m_pImgArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

}

// 设置组件样式
void MainWindow::setWidgetStyle()
{
//    this->setWindowFlags(Qt::Window);
//    this->setFixedSize(1350, 750);
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

    m_pImgArea->setFixedSize(this->width(), this->height() - m_pTitleBar->height());
    m_pImgArea->setSceneSize();

//    showMonitorSet(false, 1);
    m_pBottomBar->hide();
    m_pSideBar->setDisplayState(false);
    m_pImgArea->setShowState(true);
}

void MainWindow::setData()
{
    // 设置图片
//    m_pImgArea->loadImage("D:/image/page01.png");
//    m_pImgArea->loadImage(":/img/page01.png");


    // create folder
    QString dbFilePath      = MyDataBase::dbFilePath;
    QString imgMoldFilePath = MyDataBase::imgMoldFilePath;
    QString imgNgFilePath   = MyDataBase::imgNgFilePath;

    QDir dir(dbFilePath);
    if (!dir.exists()) {
        bool ismkdir = dir.mkdir(dbFilePath);
        ismkdir = dir.mkdir(imgMoldFilePath);
        ismkdir = dir.mkdir(imgNgFilePath);
        if(!ismkdir)
            qDebug() << "Create path fail" << endl;
        else
            qDebug() << "Create fullpath success" << endl;
    }
    dir.setPath(imgMoldFilePath);
    if (!dir.exists()) {
        dir.mkdir(imgMoldFilePath);
    }
    dir.setPath(imgNgFilePath);
    if (!dir.exists()) {
        dir.mkdir(imgNgFilePath);
    }

    // 检查NG记录文件
    QFile ngFile(MyDataBase::txtNgFilePath);
    ngFile.open(QIODevice::WriteOnly | QIODevice::Append);
    ngFile.close();

//    autoDetectImage(1, 1);
}

void MainWindow::closeEvent(QCloseEvent *)
{
    this->deleteLater();
}

void MainWindow::setRunState(int state)
{
    m_pImgArea->setRunState(state);
}

void MainWindow::showMonitorSet(bool isDisplay, int cameraId)
{
    if (isDisplay) {
        m_pBottomBar->show();
        m_pImgArea->setShowState(false);
        m_pImgArea->setMonitorState(true, cameraId);
        setDetectObject();
    } else {
        m_pBottomBar->hide();
//        ImgArea::getInstance()->clearShapes();
        m_pImgArea->setShowState(true);
        m_pImgArea->setMonitorState(false, cameraId);
    }
    m_pSideBar->setDisplayState(isDisplay);
}

void MainWindow::setDetectObject()
{
    if (m_pTitleBar->getMonitorSetState()) {
        bool isDetectMold = m_pSideBar->getIsDetectMold();
        int  curIdx = m_pSideBar->getCurrentIdx();

//        m_pImgArea->clearShapes();
        if (curIdx == 0) {
            m_pImgArea->setShowState(false);
        }

        m_pImgArea->setSampleLab(isDetectMold, curIdx);
    }
}

void MainWindow::saveMold()
{
//    QImage imageItem;
//    QList<ShapeItemData> shapeItemList;

//    imageItem = m_pImgArea->getImageItem();
//    shapeItemList = m_pImgArea->getShapeItems();

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

//    m_pImgArea->loadImageItem(imageItem);
//    m_pImgArea->loadShapeItem(shapeItemList);
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

int MainWindow::autoDetectImage(int cameraId, int sceneId)
{
    double delayTime  = 0;
    int reDetectTimes = 0;

    if (sceneId == 1) {
        delayTime = MySettings::getInstance()->getValue(SysSection, MoldDelayKey).toDouble();
        reDetectTimes = MySettings::getInstance()->getValue(SysSection, MoldTimesKey).toInt();
    } else {
        delayTime = MySettings::getInstance()->getValue(SysSection, ProdDelayKey).toDouble();
        reDetectTimes = MySettings::getInstance()->getValue(SysSection, ProdTimesKey).toInt();
    }

    qDebug() << delayTime << " " << reDetectTimes;

    m_pImgArea->autoDetectImage(cameraId, sceneId, delayTime * 1000, reDetectTimes);

    return 0;
}


