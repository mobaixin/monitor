#include <QFile>
#include <QDebug>

#include "src/view/sidebar/sidebar.h"
#include "src/view/mainwindow.h"
#include "src/view/imgarea/imgarea.h"
#include "src/view/titlebar/titlebar.h"
#include "src/serialport/myserialport.h"

#if _MSC_VER >=1600    // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif

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
    m_addMoldBtn  = new QPushButton(this);
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
    m_sideBarLayout->addWidget(m_addMoldBtn);
    m_sideBarLayout->addWidget(m_homePageBtn);
    m_sideBarLayout->addWidget(m_prevPageBtn);
    m_sideBarLayout->addWidget(m_nextPageBtn);
    m_sideBarLayout->addWidget(m_delMoldBtn);
    m_sideBarLayout->addWidget(m_clearMoldBtn);
    m_sideBarLayout->addStretch();
    m_sideBarLayout->setContentsMargins(0, 30, 0, 0);
    m_sideBarLayout->setSpacing(5);

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
    connect(m_addMoldBtn,   &QPushButton::clicked, this, &SideBar::addMoldBtnClick);
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

    m_thimbleBox->setFixedSize(100, 30);
    m_openMoldBox->setFixedSize(100, 30);
    m_canThimbleBox->setFixedSize(100, 30);
    m_canClampMoldBox->setFixedSize(100, 30);

    m_positionBtn->setFixedSize(100, 30);
    m_checkMoldBtn->setFixedSize(100, 30);
    m_productBtn->setFixedSize(100, 30);

    m_orderLab->setFixedSize(100, 20);

    m_saveMoldBtn->setFixedSize(100, 30);
    m_addMoldBtn->setFixedSize(100, 30);
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
    m_orderLab->setPalette(pe);

    m_thimbleBox->setText("  顶针");
    m_openMoldBox->setText("  开模");
    m_canThimbleBox->setText("  可顶针");
    m_canClampMoldBox->setText("  可合模");

    m_positionBtn->setText("↔");
    m_checkMoldBtn->setText("检模");
    m_productBtn->setText("产品");

    m_orderLab->setText(" 1/1");

    m_saveMoldBtn->setText("保存模板");
    m_addMoldBtn->setText("添加模板");
    m_homePageBtn->setText("首页");
    m_prevPageBtn->setText("上一页");
    m_nextPageBtn->setText("下一页");
    m_delMoldBtn->setText("删除模板");
    m_clearMoldBtn->setText("清空模板");

    setRadioBtnState(m_thimbleBox, RadioBtnState::NoState);
    setRadioBtnState(m_openMoldBox, RadioBtnState::NoState);
    setRadioBtnState(m_canThimbleBox, RadioBtnState::Correct);
    setRadioBtnState(m_canClampMoldBox, RadioBtnState::Correct);

    m_orderLab->setStyleSheet("font-size:14px;");

    QString sceneBtnStyleStr = "QPushButton{background:#00BFFF;color:#000000;border-radius:5px;font-size:14px;border: 1px groove #F3F781;}"
                               "QPushButton:hover{background:#58D3F7;color:#000000;}"
                               "QPushButton:pressed{background:#086A87;color:#FAFAFA;}"
                               "QPushButton:checked{background:#086A87;color:#FAFAFA;}";

    QString btnStyleStr = "QPushButton{background:#00BFFF;color:#000000;border-radius:5px;font-size:14px;border: 1px groove #F3F781;}"
                          "QPushButton:hover{background:#58D3F7;color:#000000;}"
                          "QPushButton:pressed{background:#00BFFF;color:#000000;}";

    m_positionBtn->setStyleSheet(btnStyleStr);
    m_checkMoldBtn->setStyleSheet(sceneBtnStyleStr);
    m_productBtn->setStyleSheet(sceneBtnStyleStr);

    m_saveMoldBtn->setStyleSheet(btnStyleStr);
    m_addMoldBtn->setStyleSheet(btnStyleStr);
    m_homePageBtn->setStyleSheet(btnStyleStr);
    m_prevPageBtn->setStyleSheet(btnStyleStr);
    m_nextPageBtn->setStyleSheet(btnStyleStr);
    m_delMoldBtn->setStyleSheet(btnStyleStr);
    m_clearMoldBtn->setStyleSheet(btnStyleStr);
}

// 设置初始数据
void SideBar::setData()
{
    m_isDetectMold = true;
    m_isShowMold   = false;
    m_sceneId      = 1;

    ImageMoldData imgData;
    imgData.cameraId = TitleBar::getInstance()->getCurCameraId();

    imgData.sceneId  = 1;
    m_deteMoldNum     = MyDataBase::getInstance()->getImageMoldNum(imgData);

    imgData.sceneId  = 2;
    m_prodMoldNum     = MyDataBase::getInstance()->getImageMoldNum(imgData);

    m_curDeteMoldIdx = m_deteMoldNum > 0 ? 1 : 0;
    m_curProdMoldIdx = m_prodMoldNum > 0 ? 1 : 0;

//    m_curDeteMoldIdx = 0;
//    m_curProdMoldIdx = 0;

    updateOrderLab();

    setDetectScene();
}

void SideBar::setDisplayState(bool isDisplay)
{
    if (isDisplay) {
        m_saveMoldBtn->show();
        m_addMoldBtn->show();
        m_homePageBtn->show();
        m_prevPageBtn->show();
        m_nextPageBtn->show();
        m_delMoldBtn->show();
        m_clearMoldBtn->show();
        m_orderLab->show();

        loadCurMold();
    } else {
        m_saveMoldBtn->hide();
        m_addMoldBtn->hide();
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

int SideBar::getCurSceneID()
{
    return m_sceneId;
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

int SideBar::getCurMoldNum()
{
    int curNum = 1;
//    if (m_isDetectMold) {
//        curNum = m_deteMoldNum;
//    } else {
//        curNum = m_prodMoldNum;
//    }

    return curNum;
}

void SideBar::saveImageMold(QGraphicsPixmapItem *imgItem)
{
    if (m_isDetectMold) {
        m_deteImgItemList.append(imgItem);
    } else {
        m_prodImgItemList.append(imgItem);
    }

    // 更新模板
    int cameraId = TitleBar::getInstance()->getCurCameraId();
//    ImgArea::getInstance()->updateShapeImgMold(cameraId, m_sceneId);
    emit updateShapeImgMoldSig(cameraId, m_sceneId);

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

void SideBar::addAlarmImageMold(QString imgPath, QString timeStr)
{
    if (m_sceneId == 1) {
        m_deteMoldNum += 1;
    } else {
        m_prodMoldNum += 1;
    }

    updateOrderLab();

    ImageMoldData imgData;
    imgData.cameraId = TitleBar::getInstance()->getCurCameraId();
    imgData.sceneId  = m_sceneId;
    imgData.moldId   = getCurMoldNum();
    imgData.imgPath  = imgPath;
    imgData.time     = timeStr;

    MyDataBase::getInstance()->addImgMoldData(imgData);
}

void SideBar::setThimbleState(int state)
{
    setRadioBtnState(m_thimbleBox, state);
}

void SideBar::setOpenMoldState(int state)
{
    setRadioBtnState(m_openMoldBox, state);
}

void SideBar::setCanThimbleState(int state)
{
    setRadioBtnState(m_canThimbleBox, state);

    if (state == RadioBtnState::Correct) {
        MySerialPort::getInstance()->writeInfo(ThimbleMask, CanThimbleValue);
    } else if (state == RadioBtnState::Wrong) {
        MySerialPort::getInstance()->writeInfo(ThimbleMask, NotThimbleValue);
    }
}

void SideBar::setCanClampMoldState(int state)
{
    setRadioBtnState(m_canClampMoldBox, state);

    if (state == RadioBtnState::Correct) {
        MySerialPort::getInstance()->writeInfo(ClampMoldMask, CanClampMoldValue);
    } else if (state == RadioBtnState::Wrong) {
        MySerialPort::getInstance()->writeInfo(ClampMoldMask, NotClampMoldValue);
    }
}

void SideBar::setDetectScene()
{
    int sceneValue = MySettings::getInstance()->getValue(SysSection, "prodDetect").toInt();

    if (sceneValue == 0) {
        if (m_sceneId == 2) {
            m_checkMoldBtn->click();
        }

        m_productBtn->hide();
    } else {
        m_productBtn->show();
    }
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
    bool isNeedUpdate = false;

    if (m_sceneId == 2) {
        isNeedUpdate = true;
    } else {
        return ;
    }

    // 清除检测结果
    ImgArea::getInstance()->clearDetectResult();

    // 切换场景时更新图形模板
    if (TitleBar::getInstance()->getMonitorSetState()) {
        updateSceneItemMold();
    }

    m_isDetectMold = true;
    m_sceneId = 1;
    MainWindow::getInstance()->setDetectObject();
//    ImgArea::getInstance()->clearShapes();
    updateOrderLab();

    if (TitleBar::getInstance()->getMonitorSetState()) {
        // 加载当前场景的模板
        loadCurMold();
    } else {
        ShapeItemData itemData;
        itemData.cameraId = TitleBar::getInstance()->getCurCameraId();
        itemData.sceneId  = m_sceneId;
        itemData.moldId   = 1;

        ImgArea::getInstance()->loadShapeItem(itemData);
    }

    OptRecord::addOptRecord("点击检模");
}

void SideBar::productBtnClick()
{
    bool isNeedUpdate = false;

    if (m_sceneId == 1) {
        isNeedUpdate = true;
    } else {
        return ;
    }

    // 清除检测结果
    ImgArea::getInstance()->clearDetectResult();

    // 切换场景时更新图形模板
    if (TitleBar::getInstance()->getMonitorSetState()) {
        updateSceneItemMold();
    }

    m_isDetectMold = false;
    m_sceneId = 2;
    MainWindow::getInstance()->setDetectObject();
//    ImgArea::getInstance()->clearShapes();
    updateOrderLab();

    if (TitleBar::getInstance()->getMonitorSetState()) {
        // 加载当前场景的模板
        loadCurMold();
    } else {
        ShapeItemData itemData;
        itemData.cameraId = TitleBar::getInstance()->getCurCameraId();
        itemData.sceneId  = m_sceneId;
        itemData.moldId   = 1;

        ImgArea::getInstance()->loadShapeItem(itemData);
    }

    OptRecord::addOptRecord("点击产品");
}

void SideBar::saveMoldBtnClick()
{
    OptRecord::addOptRecord("点击保存模板");

    if (ImgArea::getInstance()->getCameraStatus() == 0) {
        return ;
    }

    if (getCurrentIdx() == 0) {
        return ;
    }

    QDateTime time   = QDateTime::currentDateTime();
    QString fileName = time.toString("yyyy-MM-dd-HH-mm-ss");
    QString timeStr  = time.toString("yyyy-MM-dd HH:mm:ss");

    ImageMoldData imgData;
    imgData.cameraId = TitleBar::getInstance()->getCurCameraId();
    imgData.sceneId  = SideBar::getInstance()->getCurSceneID();
    imgData.moldId   = SideBar::getInstance()->getCurMoldNum();
    imgData.imgPath  = QString("%1/%2.png").arg(MyDataBase::imgMoldFilePath).arg(fileName);
    imgData.time     = timeStr;

    MyDataBase::getInstance()->addImgMoldData(imgData);

    QImage curImage = ImgArea::getInstance()->getCurImage();
    curImage.save(imgData.imgPath);

    ImgArea::getInstance()->loadImage(imgData.imgPath);

//    ShapeItemData itemData;
//    itemData.cameraId = TitleBar::getInstance()->getCurCameraId();
//    itemData.sceneId  = m_sceneId;

//    MyDataBase::getInstance()->delSceneShapeItemData(itemData);

//    qDebug() << "after delSceneShapeItemData";
//    if (m_isDetectMold) {
//        if (m_deteMoldNum == 0) {
//            m_curDeteMoldIdx = 1;
//        }
//        m_deteMoldNum += 1;
//    } else {
//        if (m_prodMoldNum == 0) {
//            m_curProdMoldIdx = 1;
//        }
//        m_prodMoldNum += 1;
//    }

//    updateOrderLab();

//    ImgArea::getInstance()->getShapeItems();
//    qDebug() << "after getShapeItems";

    // 更新模板
//    int cameraId = TitleBar::getInstance()->getCurCameraId();
//    emit updateShapeImgMoldSig(cameraId, m_sceneId);

}

void SideBar::addMoldBtnClick()
{
    OptRecord::addOptRecord("点击添加模板");

    if (ImgArea::getInstance()->getCameraStatus() == 0) {
        return ;
    }

    bool isFirstAdd = false;
    if (m_isDetectMold) {
        if (m_deteMoldNum == 0) {
            m_curDeteMoldIdx = 1;
            isFirstAdd = true;
        }
        m_deteMoldNum += 1;
    } else {
        if (m_prodMoldNum == 0) {
            m_curProdMoldIdx = 1;
            isFirstAdd = true;
        }
        m_prodMoldNum += 1;
    }

    MainWindow::getInstance()->setDetectObject();
    updateOrderLab();

    QImage curImage = ImgArea::getInstance()->getImageItem();

    if (isFirstAdd) {
        loadCurImage();
    }

    // 更新模板
    int cameraId = TitleBar::getInstance()->getCurCameraId();
    emit updateShapeImgMoldSig(cameraId, m_sceneId);

    return ;
}

void SideBar::homePageBtnClick()
{
    OptRecord::addOptRecord("点击首页");

    if (m_isDetectMold) {
        if (m_deteMoldNum > 0) {
            m_curDeteMoldIdx = 1;
        } else {
            return ;
        }
    } else {
        if (m_prodMoldNum > 0) {
            m_curProdMoldIdx = 1;
        } else {
            return ;
        }
    }
    MainWindow::getInstance()->setDetectObject();
    updateOrderLab();

    loadCurImage();
}

void SideBar::prevPageBtnClick()
{
    OptRecord::addOptRecord("点击前一页");

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

        loadCurImage();
    }

}

void SideBar::nextPageBtnClick()
{
    OptRecord::addOptRecord("点击后一页");

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

        loadCurImage();
    }
}

void SideBar::delMoldBtnClick()
{
    OptRecord::addOptRecord("点击删除模板");

    bool isChange = false;
    ImageMoldData imgData;
    imgData.cameraId = TitleBar::getInstance()->getCurCameraId();
    imgData.sceneId  = m_sceneId;
    imgData.moldId   = getCurrentIdx();

    ImageMoldData resImgData = MyDataBase::getInstance()->queImgMoldData(imgData);
    QFile imgMoldFile(resImgData.imgPath);
    if (imgMoldFile.exists()) {
        imgMoldFile.remove();
    }

    if (m_isDetectMold) {
        if (m_deteMoldNum > 0) {
            m_deteImgItemList.removeAt(m_curDeteMoldIdx - 1);
            m_deteShapeItemList.removeAt(m_curDeteMoldIdx - 1);

            if (m_curDeteMoldIdx == m_deteMoldNum) {
                m_curDeteMoldIdx -= 1;
                isChange = true;
            }
            MyDataBase::getInstance()->delImgMoldData(imgData);
            MyDataBase::getInstance()->updateImgMoldId(imgData);
            m_deteMoldNum -= 1;
        }
    } else {
        if (m_prodMoldNum > 0) {
            m_prodImgItemList.removeAt(m_curProdMoldIdx - 1);
            m_prodShapeItemList.removeAt(m_curProdMoldIdx - 1);

            if (m_curProdMoldIdx == m_prodMoldNum) {
                m_curProdMoldIdx -= 1;
                isChange = true;
            }
            MyDataBase::getInstance()->delImgMoldData(imgData);
            MyDataBase::getInstance()->updateImgMoldId(imgData);
            m_prodMoldNum -= 1;
        }
    }
    updateOrderLab();
    MainWindow::getInstance()->setDetectObject();

    if (isChange) {
        loadCurImage();
    }
}

void SideBar::clearMoldBtnClick()
{
    OptRecord::addOptRecord("点击清空模板");

    ImageMoldData imgData;
    imgData.cameraId = TitleBar::getInstance()->getCurCameraId();
    imgData.sceneId  = m_sceneId;
    imgData.moldId   = getCurrentIdx();

    QList<ImageMoldData> resImgDataList = MyDataBase::getInstance()->queAllImgMoldData(imgData);
    QFile imgMoldFile;
    for (int i = 0; i < resImgDataList.size(); i++) {
        imgMoldFile.setFileName(resImgDataList[i].imgPath);
        if (imgMoldFile.exists()) {
            imgMoldFile.remove();
        }
    }

    if (m_isDetectMold) {
        m_deteImgItemList.clear();
        m_deteShapeItemList.clear();

        m_curDeteMoldIdx = 0;
        m_deteMoldNum = 0;
    } else {
        m_prodImgItemList.clear();
        m_prodShapeItemList.clear();

        m_curProdMoldIdx = 0;
        m_prodMoldNum = 0;
    }
    updateOrderLab();
    MainWindow::getInstance()->setDetectObject();

//    ImageMoldData imgData;
//    imgData.cameraId = TitleBar::getInstance()->getCurCameraId();
//    imgData.sceneId  = m_isDetectMold ? 1 : 2;

    MyDataBase::getInstance()->delSceneImgMoldData(imgData);

    loadCurImage();
}

void SideBar::updateOrderLab()
{
    QString text;
    if (m_isDetectMold) {
        text = QString(" %1/%2").arg(m_curDeteMoldIdx).arg(m_deteMoldNum);
    } else {
        text = QString(" %1/%2").arg(m_curProdMoldIdx).arg(m_prodMoldNum);
    }
    m_orderLab->setText(text);
}

void SideBar::updateSceneItemMold()
{
    int cameraId = TitleBar::getInstance()->getCurCameraId();
    int sceneId  = m_sceneId;

    ShapeItemData itemData;
    itemData.cameraId = cameraId;
    itemData.sceneId  = sceneId;

    // 清理数据库的图形模板
    MyDataBase::getInstance()->delSceneShapeItemData(itemData);

    // 更新数据库的图形模板
    ImgArea::getInstance()->getShapeItems();

    // 更新模板
//    emit updateShapeImgMoldSig(cameraId, sceneId);
}

void SideBar::loadCurMold()
{
    ShapeItemData itemData;
    itemData.cameraId = TitleBar::getInstance()->getCurCameraId();
    itemData.sceneId  = m_sceneId;
    itemData.moldId   = 1;

    ImgArea::getInstance()->loadShapeItem(itemData);

    if (getCurrentIdx() > 0) {

        ImageMoldData imgData;
        imgData.cameraId = TitleBar::getInstance()->getCurCameraId();
        imgData.sceneId  = m_sceneId;
        imgData.moldId   = getCurrentIdx();

        ImgArea::getInstance()->loadImageItem(imgData);
    }
}

void SideBar::loadCurImage()
{
    ImageMoldData imgData;
    imgData.cameraId = TitleBar::getInstance()->getCurCameraId();
    imgData.sceneId  = m_sceneId;
    imgData.moldId   = getCurrentIdx();

    qDebug() << "load img";
    ImgArea::getInstance()->loadImageItem(imgData);
}

void SideBar::setRadioBtnState(QRadioButton *btn, int state)
{
    QString noStateStyleStr = "QRadioButton{font-size:14px;}"
                              "QRadioButton::indicator{background:#D8D8D8;width:16px;height:16px;border-radius:8px;}";
    QString correctStyleStr = "QRadioButton{font-size:14px;}"
                              "QRadioButton::indicator{background-color:#04B404;width:16px;height:16px;border-radius:8px;}";
    QString wrongStyleStr   = "QRadioButton{font-size:14px;"
                              "}QRadioButton::indicator{background-color:#DF0101;width:16px;height:16px;border-radius:8px;}";

    if (state == RadioBtnState::NoState) {
        btn->setStyleSheet(noStateStyleStr);
    } else if (state == RadioBtnState::Correct) {
        btn->setStyleSheet(correctStyleStr);
    } else if (state == RadioBtnState::Wrong) {
        btn->setStyleSheet(wrongStyleStr);
    }
}
