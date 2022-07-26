#include <QMessageBox>
#include <QPixmap>
#include <QDebug>
#include <QApplication>
#include <QList>
#include <QDateTime>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QProcess>

#include "src/view/imgarea/imgarea.h"
#include "src/view/mainwindow.h"
#include "src/database/mydatabase.h"
#include "src/view/sidebar/sidebar.h"
#include "src/view/bottombar/bottombar.h"

#if _MSC_VER >=1600    // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif

#ifdef _WIN64
#pragma comment(lib,"D:\\Documents\\QTProjects\\monitor\\src\\camera\\lib\\MVCAMSDK_X64.lib")
#else
#pragma comment(lib,"D:\\Documents\\QTProjects\\monitor\\src\\camera\\lib\\MVCAMSDK.lib")
#endif

// SDK
int                     g_hCamera = -1;     //设备句柄
unsigned char           * g_pRawBuffer=NULL;     //raw数据
unsigned char           * g_pRgbBuffer=NULL;     //处理后数据缓存区
tSdkFrameHead           g_tFrameHead;       //图像帧头信息
tSdkCameraCapbility     g_tCapability;      //设备描述信息

int                     g_SaveParameter_num=0;    //保存参数组
int                     g_SaveImage_type=0;         //保存图像格式

Width_Height            g_W_H_INFO;         //显示画板到大小和图像大小
BYTE                    *g_readBuf=NULL;    //画板显示数据区
int                     g_read_fps=0;       //统计读取帧率
int                     g_disply_fps=0;     //统计显示帧率

ImgArea *ImgArea::getInstance(QWidget *parent)
{
    static ImgArea *instance = nullptr;
    if (nullptr == instance) {
        instance = new ImgArea(parent);
    }
    return instance;
}

ImgArea::ImgArea(QWidget *parent)
    : QWidget(parent)
{
    // 初始化组件
    setWidgetUi();

    // 设置组件样式
    setWidgetStyle();

    // 设置初始数据
    setData();
}

ImgArea::~ImgArea()
{
    qDebug() << "~ImgArea";

    m_thread->stop();
    while (!m_thread->wait(100) )
    {
        QApplication::processEvents();
    }

    if(g_readBuf!=NULL){
        free(g_readBuf);
        g_readBuf=NULL;
    }

    if(g_pRgbBuffer!=NULL){
        free(g_pRgbBuffer);
        g_pRgbBuffer=NULL;
    }

    if(g_hCamera>0){
        //相机反初始化。释放资源。
        CameraUnInit(g_hCamera);
        g_hCamera=-1;
    }

    delete  m_pMainImg;

    m_detectThread->quit();
    m_detectThread->wait();
}

void ImgArea::setWidgetUi()
{
    qRegisterMetaType<QList<ShapeItemData>>("QList<ShapeItemData>");
    qRegisterMetaType<QList<ImageMoldData>>("QList<ImageMoldData>");
    qRegisterMetaType<QVector<QVector<QPointF>>>("QVector<QVector<QPointF>>");
    qRegisterMetaType<QList<int>>("QList<int>");

    m_pMainImg = new QImage;

    m_pImgAreaLayout = new QHBoxLayout(this);

    m_pView  = new QGraphicsView();
    m_pScene = new MyGraphicsScene();
    m_pImageItem = new QGraphicsPixmapItem();

    m_resTimer = new QTimer(this);
    m_thread   = new CaptureThread(this);

    m_sigDelayTimer = new QTimer(this);

    // 图片检测线程
    m_detectThread = new QThread(this);
    m_detectImageWork = new DetectImageWork();
    m_detectImageWork->moveToThread(m_detectThread);

    m_pView->setScene(m_pScene);
    m_pView->setRenderHint(QPainter::Antialiasing);
//    m_pView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    m_pView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pView->centerOn(0, 0);
    m_pView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
//    m_pView->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    m_pScene->addItem(m_pImageItem);
    m_pScene->setBackgroundBrush(Qt::black);

    m_pImgAreaLayout->addWidget(m_pView);
    m_pImgAreaLayout->setContentsMargins(0, 0, 0, 0);
    m_pImgAreaLayout->setSpacing(0);

    this->setLayout(m_pImgAreaLayout);

    m_pTipLab     = new QLabel(this);
    m_pSigTimeLab = new QLabel(this);
    m_pSampleLab  = new QLabel(this);
    m_pResultLab  = new QLabel(this);

    m_pSigTimeLab->hide();
    m_pSampleLab->hide();
    m_pResultLab->hide();

    connect(m_resTimer, &QTimer::timeout, [=](){
        m_resTimer->stop();
        m_pResultLab->hide();
    });

    connect(m_sigDelayTimer, &QTimer::timeout, this, &ImgArea::setSigDelayTimeLab);

    connect(m_thread, SIGNAL(captured(QImage)),
            this, SLOT(imageProcess(QImage)), Qt::BlockingQueuedConnection);

    connect(this, &ImgArea::setCameraCountsSig,
            m_detectImageWork, &DetectImageWork::setCameraCounts, Qt::BlockingQueuedConnection);

    connect(this, &ImgArea::setSceneRectSizeSig,
            m_detectImageWork, &DetectImageWork::setSceneRectSize, Qt::BlockingQueuedConnection);

    connect(this, &ImgArea::startUpdateMoldSig,
            m_detectImageWork, &DetectImageWork::updateShapeImgMold, Qt::QueuedConnection);

    connect(this, &ImgArea::startDetectImageSig,
            m_detectImageWork, &DetectImageWork::detectImage, Qt::BlockingQueuedConnection);

    connect(m_detectThread, &QThread::finished, m_detectImageWork, &QObject::deleteLater);

    connect(m_detectImageWork, &DetectImageWork::resultReadySig,
            this, &ImgArea::getDetectResult, Qt::DirectConnection);

    m_detectThread->start();
}

void ImgArea::setWidgetStyle()
{
    m_pTipLab->setFixedSize(200, 30);
    m_pTipLab->setGeometry(10, 0, m_pTipLab->width(), m_pTipLab->height());

    m_pSigTimeLab->setFixedSize(200, 30);
    m_pSigTimeLab->setGeometry(10, 25, m_pSigTimeLab->width(), m_pSigTimeLab->height());

    m_pSampleLab->setFixedSize(500, 30);
    m_pSampleLab->setGeometry(700, 20, m_pTipLab->width(), m_pTipLab->height());

    m_pResultLab->setFixedSize(200, 30);
    m_pResultLab->setGeometry(1000, 20, m_pResultLab->width(), m_pResultLab->height());

    m_pSigTimeLab->setStyleSheet("font-family:Microsoft YaHei;font-size:16px;color:green;");

//    m_pSampleLab->setText("检模取样图像:第1张");
    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::yellow);
    m_pSampleLab->setPalette(pe);

    QFont sampleFont = this->font();
    sampleFont.setPixelSize(25);
    sampleFont.setBold(true);
    sampleFont.setFamily("Microsoft YaHei");

    m_pSampleLab->setFont(sampleFont);
//    m_pSampleLab->setStyleSheet("font-weight:50;");

    m_pResultLab->setText("检模OK");

}

void ImgArea::setData()
{
    m_isShowImage = true;

//    status =0;
//    setRunState(CameraState::OffLine);

    // 初始化本机IP地址
    initLocalNetwork();

    int maxInitTimes = 3;
    for (int i = 0; i < maxInitTimes; i++) {
        int initRes = initSDK();

        if(initRes == -1){
            status =0;
            setRunState(CameraState::OffLine);

        // 相机IP配置失败
        } else if (initRes == -2) {
            QThread::msleep(2000);

            if (i == 2) {
                status =0;
                setRunState(CameraState::OffLine);
            }

            continue ;

        // 相机IP配置成功 需要再次初始化
        } else if (initRes == 2) {
            if (maxInitTimes == 3 ) {
                maxInitTimes = 4;
            } else {
                status =0;
                setRunState(CameraState::OffLine);
            }

            continue ;
        }
        else {
            qDebug() << "before initParameter";
            initParameter(g_hCamera,&g_tCapability);
//            CameraSetOnceWB(g_hCamera);
            qDebug() << "after initParameter";
            m_thread->start();
            m_thread->stream();
            status = 1;
            setRunState(CameraState::Running);
        }
        break;
    }

    qDebug() << "status: " << status;

    // 获取设置的相机数
    int count = MySettings::getInstance()->getValue(SysSection, "cameraCounts").toInt();
    emit setCameraCountsSig(count);

    CameraDetectData cameraData;
    m_cameraDetectDataList.append(cameraData);
//    updateShapeImgMold(1, 1);
//    updateShapeImgMold(1, 2);
}

MyGraphicsScene *ImgArea::getScene()
{
    return m_pScene;
}

void ImgArea::loadImage(QString filePath)
{
    if (!(m_pMainImg->load(filePath)))
    {
        QMessageBox::warning(this, "错误", "打开图像失败！");
        return;
    }
    QPixmap imgPix = (QPixmap::fromImage(*m_pMainImg));
    imgPix = imgPix.scaled(this->size());
    m_pImageItem->setPixmap(imgPix);
    m_pImageItem->show();
}

void ImgArea::eraseShape()
{
    if (!m_pScene->selectedItems().isEmpty()) {
        QGraphicsItem *temp = m_pScene->selectedItems().first();

        bool isPoint = false;
        MyPointItem *item = static_cast<MyPointItem *>(temp);

        switch (item->getPointType()) {
        case MyPointItem::Center:
        case MyPointItem::Edge:
        case MyPointItem::Special:
        case MyPointItem::Other:
            isPoint = true;
            break;
        default:
            break;
        }

        if (isPoint) {
            return ;
        }

        // 清除图形 回收内存
        m_pScene->removeItem(temp);

        MyGraphicsItem *shapeItem = static_cast<MyGraphicsItem *>(temp);
        shapeItem->deleteLater();
//        delete temp;
    }
}

void ImgArea::clearShapes()
{
    qDebug() << "in clearShapes";
    QList<QGraphicsItem *> itemList = m_pScene->items();
    qDebug() << "itemList.size: " << itemList.size();

    if (itemList.size() == 1) {
        return ;
    }

    QList<int> clearItemIdxList;
    for (int i = itemList.size() - 1; i >= 0; i--) {
//        if (itemList[i] != m_pImageItem && itemList[i] != nullptr && itemList[i]->scene() != nullptr && itemList[i]->parentItem() == nullptr) {
        if (itemList[i] != m_pImageItem && itemList[i] != nullptr) {

            // todo
            bool isPoint = false;
            MyPointItem *item = static_cast<MyPointItem *>(itemList[i]);

            switch (item->getPointType()) {
            case MyPointItem::Center:
            case MyPointItem::Edge:
            case MyPointItem::Special:
            case MyPointItem::Other:
                isPoint = true;
                break;
            default:
                break;
            }

            if (isPoint) {
                continue ;
            }

//            qDebug() << "item->getPointType(): " << item->getPointType();
            clearItemIdxList.append(i);
        }
    }

    // 清除图形 回收内存
    for (int i = clearItemIdxList.size() - 1; i >= 0; i--) {
        m_pScene->removeItem(itemList[clearItemIdxList[i]]);

        MyGraphicsItem *shapeItem = static_cast<MyGraphicsItem *>(itemList[i]);
        shapeItem->deleteLater();
//        delete itemList[clearItemIdxList[i]];
    }

    qDebug() << "out clearShapes";
}

void ImgArea::setRunState(int state)
{
    if (state == CameraState::Running && status == 0) {
        state = CameraState::OffLine;
    }

    QString runStyleStr = "color:#80FF00;font-size:20px;font-weight:10px;";
    QString pauStyleStr = "color:red;font-size:20px;font-weight:10px;";

    switch (state) {
    case CameraState::OffLine: {
        m_pTipLab->setText(QString("相机断线"));
        m_pTipLab->setStyleSheet(pauStyleStr);
    }
        break;
    case CameraState::Running: {
        m_pTipLab->setText(QString("运行监视中"));
        m_pTipLab->setStyleSheet(runStyleStr);
    }
        break;
    case CameraState::Pause: {
        m_pTipLab->setText(QString("保护停止"));
        m_pTipLab->setStyleSheet(pauStyleStr);
    }
        break;
    default:
        break;
    }
}

void ImgArea::setMonitorState(bool isMonitor)
{
    if (isMonitor) {
        m_pSampleLab->show();
    } else {
        m_pSampleLab->hide();

        if (!m_thread->isRunning()) {
            QPixmap imgPix = (QPixmap::fromImage(m_pCurImage));
            imgPix = imgPix.scaled(this->size());
            m_pImageItem->setPixmap(imgPix);
        }
    }
}

void ImgArea::setDetectState(bool isDetect)
{
    if (isDetect) {
        m_pResultLab->show();
    } else {
        m_pResultLab->hide();
    }
}

void ImgArea::setSceneSize()
{
    qDebug() << "setSceneSize " << this->width() << " " << this->height();
    m_sceneSize = QSize(this->width(), this->height());
    m_pView->setSceneRect(0, 0, this->width() - 5, this->height() - 5);

    emit setSceneRectSizeSig(m_sceneSize);

    // 加载图形模板
    ShapeItemData itemData;
    itemData.cameraId = TitleBar::getInstance()->getCurCameraId();
    itemData.sceneId  = SideBar::getInstance()->getCurSceneID();
    itemData.moldId   = 1;

    loadShapeItem(itemData);
    setShapeNoMove(true);

    updateShapeImgMold(1, 1);
    updateShapeImgMold(1, 2);

    connect(SideBar::getInstance(), &SideBar::updateShapeImgMoldSig, this, &ImgArea::updateShapeImgMold);
}

void ImgArea::setDetectRes(bool isOK, int sceneId)
{
    if (sceneId == -1) {
        sceneId = SideBar::getInstance()->getCurSceneID();
    }

    QString sceneName = sceneId == 1 ? "检模" : "产品";

    if (isOK) {
        m_pResultLab->setText(QString("%1OK").arg(sceneName));
        m_pResultLab->setStyleSheet("color:#80FF00;font-family:Microsoft YaHei;font-size:20px;font-weight:10px;");

        m_resTimer->start(3000);

    } else {
        m_pResultLab->setText(QString("%1NG").arg(sceneName));
        m_pResultLab->setStyleSheet("color:red;font-family:Microsoft YaHei;font-size:20px;font-weight:10px;");
    }
    m_pResultLab->show();
}

void ImgArea::setSampleLab(bool isDetectMold, int curIdx)
{
    if (isDetectMold) {
        m_pSampleLab->clear();
        m_pSampleLab->setText(QString("检模取样图像:第%1张").arg(curIdx));
    } else {
        m_pSampleLab->clear();
        m_pSampleLab->setText(QString("产品取样图像:第%1张").arg(curIdx));
    }
    m_pSampleLab->show();
}

QImage ImgArea::getImageItem()
{
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
    m_pCurImage.save(imgData.imgPath);

    return m_pCurImage;
}

QList<ShapeItemData> ImgArea::getShapeItems()
{
    qDebug() << "getShapeItems: " << m_pScene->items().size();
    QList<ShapeItemData> shapeList;
    for (QGraphicsItem *temp : m_pScene->items()) {
        if (temp != m_pImageItem && temp != nullptr && temp->scene() != nullptr) {

//            qDebug() << "1";
            MyGraphicsItem *item = static_cast<MyGraphicsItem *>(temp);
            QPoint center = item->getRealCenter().toPoint();
//            qDebug() << "2";

            ShapeItemData itemData;
            itemData.cameraId = TitleBar::getInstance()->getCurCameraId();
            itemData.sceneId  = SideBar::getInstance()->getCurSceneID();
            itemData.moldId   = SideBar::getInstance()->getCurrentIdx();
            itemData.type     = item->getType();
            itemData.center   = QString("%1,%2").arg(center.x()).arg(center.y());
            itemData.accuracy = item->getAccuracy();
            itemData.pixel    = item->getPixel();
//            qDebug() << "3";
//            qDebug() << "item->getType(): " << item->getType();

            switch (item->getType()) {
            case MyGraphicsItem::ItemType::Rectangle:{
//                qDebug() << "item type: " << item->getType();
//                qDebug() << "real center:" << item->getRealCenter().x() << " " << item->getRealCenter().y();
//                qDebug() << "center" << item->getCenter().x() << " " << item->getCenter().y();
//                qDebug() << "edge" << item->getEdge().x() << " " << item->getEdge().y();

                int width  = abs(item->getEdge().x() - item->getCenter().x()) * 2;
                int height = abs(item->getEdge().y() - item->getCenter().y()) * 2;

                // 记录矩形的长和宽
                QPointF edge = QPointF(width, height);

                QList<QPointF> myList;
                myList.append(QPointF(center.x() - width/2, center.y() - height/2));
                myList.append(QPointF(center.x() + width/2, center.y() + height/2));

                itemData.edge      = QString("%1,%2").arg(edge.x()).arg(edge.y());
                itemData.pointList = pointListToStr(myList);

                MyDataBase::getInstance()->addShapeItemData(itemData);
            }
                break;
            case MyGraphicsItem::ItemType::Polygon:
            case MyGraphicsItem::ItemType::Polygon_Mask: {
                QList<QPointF> myList = item->getMyPointList();
                qDebug() << "myList.size: " << myList.size();
                myList.removeLast();

                itemData.edge      = QString("");
                itemData.pointList = pointListToStr(myList);

                MyDataBase::getInstance()->addShapeItemData(itemData);
            }
                break;
            case MyGraphicsItem::ItemType::Circle: {
                int radius = sqrt(pow(item->getCenter().x() - item->getEdge().x(), 2) + pow(item->getCenter().y() - item->getEdge().y(), 2));

                // 记录圆形的半径
                QPointF edge = QPointF(radius, radius);
                itemData.edge      = QString("%1,%2").arg(edge.x()).arg(edge.y());

                MyDataBase::getInstance()->addShapeItemData(itemData);
            }
                break;
            case MyGraphicsItem::ItemType::Concentric_Circle: {
                QList<QPointF> myList = item->getMyPointList();

                if (myList.size() < 3) {
                    break;
                }

                // 记录内外圆的半径
                int radius1 = sqrt(pow(myList[1].x() - myList[0].x(), 2) + pow(myList[1].y() - myList[0].y(), 2));
                int radius2 = sqrt(pow(myList[1].x() - myList[2].x(), 2) + pow(myList[1].y() - myList[2].y(), 2));

                itemData.edge = QString("%1,%2").arg(radius1).arg(radius2);

                MyDataBase::getInstance()->addShapeItemData(itemData);
            }
                break;
            case MyGraphicsItem::ItemType::Curve: {
                QList<QPointF> myList = item->getMyPointList();
                qDebug() << "Curve myList.size: " << myList.size();
                if (myList.size() < 1) {
                    break;
                }

                myList.removeLast();

                itemData.edge      = QString("");
                itemData.pointList = pointListToStr(myList);

                MyDataBase::getInstance()->addShapeItemData(itemData);
            }
                break;
            default:
                break;

            }
        }
    }

    // 更新模板
    int cameraId = TitleBar::getInstance()->getCurCameraId();
    int sceneId  = SideBar::getInstance()->getCurSceneID();
    updateShapeImgMold(cameraId, sceneId);

    return shapeList;
}

void ImgArea::loadImageItem(ImageMoldData imgData)
{
    if (imgData.moldId <= 0) {
        m_pImageItem->hide();
        return ;
    }

    ImageMoldData resImgData =MyDataBase::getInstance()->queImgMoldData(imgData);
    QImage img;

//    qDebug() << "image path: " << resImgData.imgPath;
    if (img.load(resImgData.imgPath)) {
        QPixmap imgPix = (QPixmap::fromImage(img));
        imgPix = imgPix.scaled(this->size());
        m_pImageItem->setPixmap(imgPix);

//        qDebug() << "show image";
        m_pImageItem->show();
    }
}

void ImgArea::loadShapeItem(ShapeItemData itemData)
{
    clearShapes();

    QList<ShapeItemData> itemDataList = MyDataBase::getInstance()->queShapeItemData(itemData);

    for (int i = 0; i < itemDataList.size(); i++) {
        QStringList centerList = itemDataList[i].center.split(',');
        QPointF center;
        if (centerList.size() >= 2) {
            center = QPointF(centerList[0].toInt(), centerList[1].toInt());
        }

        switch (itemDataList[i].type) {
        case MyGraphicsItem::ItemType::Rectangle: {
            QStringList edgeList = itemDataList[i].edge.split(',');
            QPointF edge;
            if (edgeList.size() >= 2) {
                edge = QPointF(edgeList[0].toInt(), edgeList[1].toInt());
            }

            MyRectangle *myRect = new MyRectangle(center.x(), center.y(), edge.x(), edge.y(), MyGraphicsItem::ItemType::Rectangle);
            myRect->setAccuracy(itemDataList[i].accuracy);
            myRect->setPixel(itemDataList[i].pixel);
            m_pScene->addItem(myRect);
        }
            break;
        case MyGraphicsItem::ItemType::Polygon:
        case MyGraphicsItem::ItemType::Polygon_Mask: {
            QStringList myStrList = itemDataList[i].pointList.split(',');
            QPointF myEdgePoint;
            QList<QPointF> myEdgePointList;

            if (myStrList.size() > 2) {
                MyPolygon *myPolygon;

                if (itemDataList[i].type == MyGraphicsItem::ItemType::Polygon) {
                    myPolygon = new MyPolygon(MyGraphicsItem::ItemType::Polygon);
                } else {
                    myPolygon = new MyPolygon(MyGraphicsItem::ItemType::Polygon_Mask);
                }

                for (int i = 0; i < myStrList.size() - 1; i+=2) {
                    if (i + 1 < myStrList.size()) {
                        myEdgePoint = QPointF(myStrList[i].toInt(), myStrList[i+1].toInt());
                        myEdgePointList.append(myEdgePoint);
                        myPolygon->pushPoint(myEdgePoint, myEdgePointList, false);
                    }
                }
                myPolygon->pushPoint(myEdgePoint, myEdgePointList, true);
                myPolygon->setAccuracy(itemDataList[i].accuracy);
                myPolygon->setPixel(itemDataList[i].pixel);
                m_pScene->addItem(myPolygon);
                connect(m_pScene, &MyGraphicsScene::updatePolyPoint, myPolygon, &MyPolygon::pushPoint);
            }
        }
            break;
        case MyGraphicsItem::ItemType::Curve: {
            QStringList myStrList = itemDataList[i].pointList.split(',');
            QPointF myEdgePoint;
            QList<QPointF> myEdgePointList;

            if (myStrList.size() > 2) {
                MyCurve *myCurve = new MyCurve(MyGraphicsItem::ItemType::Curve);

                for (int i = 0; i < myStrList.size() - 1; i+=2) {
                    if (i + 1 < myStrList.size()) {
                        myEdgePoint = QPointF(myStrList[i].toInt(), myStrList[i+1].toInt());
                        myEdgePointList.append(myEdgePoint);
                        myCurve->pushPoint(myEdgePoint, myEdgePointList, false);
                    }
                }
                myCurve->pushPoint(myEdgePoint, myEdgePointList, true);
                myCurve->setAccuracy(itemDataList[i].accuracy);
                myCurve->setPixel(itemDataList[i].pixel);
                m_pScene->addItem(myCurve);
            }
        }
            break;
        case MyGraphicsItem::ItemType::Circle: {
            QStringList edgeList = itemDataList[i].edge.split(',');
            QPointF edge;
            if (edgeList.size() >= 2) {
                edge = QPointF(edgeList[0].toInt(), edgeList[1].toInt());
            }

            MyCircle *myCircle = new MyCircle(center.x(), center.y(), edge.x(), MyGraphicsItem::ItemType::Circle);
            myCircle->setAccuracy(itemDataList[i].accuracy);
            myCircle->setPixel(itemDataList[i].pixel);
            m_pScene->addItem(myCircle);
        }
            break;
        case MyGraphicsItem::ItemType::Concentric_Circle: {
            QStringList edgeList = itemDataList[i].edge.split(',');
            QPointF edge;
            if (edgeList.size() >= 2) {
                edge = QPointF(edgeList[0].toInt(), edgeList[1].toInt());
            }

            MyConcentricCircle *myConCircle = new MyConcentricCircle(center.x(), center.y(), edge.x(), edge.y(), MyGraphicsItem::ItemType::Concentric_Circle);
            myConCircle->setAccuracy(itemDataList[i].accuracy);
            myConCircle->setPixel(itemDataList[i].pixel);
            m_pScene->addItem(myConCircle);
        }
            break;
        default:
            break;
        }
    }
}

QImage ImgArea::saveAsImage(QString imgPath)
{
    qDebug() << m_sceneSize.width() << " " << m_sceneSize.height();
    QImage image(m_sceneSize, QImage::Format_RGB32);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    m_pScene->render(&painter);

    image.save(imgPath);

    return image;
}

// 设置分辨率
void ImgArea::setResolution(int index)
{
    tSdkImageResolution   *pImageSizeDesc=g_tCapability.pImageSizeDesc;// 预设分辨率选择

    g_W_H_INFO.sensor_width=pImageSizeDesc[index].iWidth;
    g_W_H_INFO.sensor_height=pImageSizeDesc[index].iHeight;
    g_W_H_INFO.buffer_size= g_W_H_INFO.sensor_width*g_W_H_INFO.sensor_height;

    if(pImageSizeDesc[index].iWidthZoomSw){
        g_W_H_INFO.sensor_width=pImageSizeDesc[index].iWidthZoomSw;
        g_W_H_INFO.sensor_height=pImageSizeDesc[index].iHeightZoomSw;
    }

    if(g_W_H_INFO.sensor_width < g_W_H_INFO.display_width){
        g_W_H_INFO.xOffsetFOV=(g_W_H_INFO.display_width-g_W_H_INFO.sensor_width)/2;
    }else{
        g_W_H_INFO.xOffsetFOV=0;
    }
    if(g_W_H_INFO.sensor_height < g_W_H_INFO.display_height){
        g_W_H_INFO.yOffsetFOV=(g_W_H_INFO.display_height-g_W_H_INFO.sensor_height)/2;
    }else{
        g_W_H_INFO.yOffsetFOV=0;
    }
    m_thread->pause();
    //设置预览的分辨率。
    CameraSetImageResolution(g_hCamera,&(pImageSizeDesc[index]));
    m_thread->stream();
}

void ImgArea::setExposeTime(int value, int cameraIdx)
{
    Q_UNUSED(cameraIdx);
    double m_fExpLineTime = 0;  //当前的行曝光时间，单位为us
    CameraGetExposureLineTime(g_hCamera, &m_fExpLineTime);
    CameraSetExposureTime(g_hCamera, value * m_fExpLineTime);

    m_pfExposureTime = value * m_fExpLineTime;
}

void ImgArea::setCameraGain(int gain, int cameraIdx)
{
    Q_UNUSED(cameraIdx);
    CameraSetAnalogGain(g_hCamera, gain);
    m_pusAnalogGain = gain;
}

QList<double> ImgArea::getExposureTime()
{
    QList<double> exposureTimeList;

    exposureTimeList.append(m_pfExposureTime);
    exposureTimeList.append(m_exposureTimeMin);
    exposureTimeList.append(m_exposureTimeMax);
    exposureTimeList.append(m_expLineTime);

    return exposureTimeList;
}

QList<int> ImgArea::getCameraGain()
{
    QList<int> cameraGainList;

    cameraGainList.append(m_pusAnalogGain);
    cameraGainList.append(m_analogGainMin);
    cameraGainList.append(m_analogGainMax);

    return cameraGainList;
}

void ImgArea::saveCameraPara()
{
    CameraSaveParameter(g_hCamera,g_SaveParameter_num);
}

void ImgArea::startCamera()
{
    if (m_thread->quit) {
        return ;
    }
    m_thread->stream();
}

void ImgArea::pauseCamera()
{
    if (m_thread->quit) {
        return ;
    }
    m_thread->pause();
}

// SDK初始化
int ImgArea::initSDK()
{
    m_cameraCounts = 4;
    int               iStatus=-1;
    tSdkCameraDevInfo tCameraEnumList[4];

    //sdk初始化  0 English 1中文
    CameraSdkInit(1);

    //枚举设备，并建立设备列表
    CameraEnumerateDevice(tCameraEnumList,&m_cameraCounts);

    qDebug() << "CameraCounts: " << m_cameraCounts;

    //没有连接设备
    if(m_cameraCounts==0){
        return -1;
    }

    // 获取相机IP信息
    char* ipInfo[6];
    for (int i = 0; i < 6; i++) {
        ipInfo[i] = QString("0").toUtf8().data();
    }

    CameraGigeGetIp(&tCameraEnumList[0], ipInfo[0], ipInfo[1], ipInfo[2], ipInfo[3], ipInfo[4], ipInfo[5]);

    // 设置相机IP
    QString cameraIp = m_cameraIp.arg(QString(ipInfo[3]).right(1)).arg(QString(ipInfo[3]).right(1));
    qDebug() << "cameraIP: " << cameraIp;

    // 数据库交互
    CameraIPData cameraIPData;
    cameraIPData.cameraId = 1;
    cameraIPData.serialId = QString(tCameraEnumList[0].acSn);
    cameraIPData.nickName = QString(tCameraEnumList[0].acFriendlyName);
    cameraIPData.portIp   = QString(ipInfo[3]);
    cameraIPData.state    = "可用";
    cameraIPData.cameraIp = cameraIp;
    cameraIPData.cameraMask    = QString(ipInfo[4]);
    cameraIPData.cameraGateway = QString(ipInfo[5]);

    // 判断本机IP和相机IP
    if (QString(ipInfo[0]) != cameraIp) {
        int res = CameraGigeSetIp(&tCameraEnumList[0], (cameraIp.toLatin1()).data(), (QString(ipInfo[4]).toLatin1()).data(),
                                 (QString(ipInfo[5]).toLatin1()).data(), true);
        if (res == CAMERA_STATUS_SUCCESS) {
            qDebug() << "相机IP设置成功";
            cameraIPData.state = "可用";
        } else {
            qDebug() << "相机IP设置失败";
            cameraIPData.state = "不可用";
        }

        // 数据库交互
        if (MyDataBase::getInstance()->queCameraIPData(cameraIPData).cameraId == -1) {
            MyDataBase::getInstance()->addCameraIPData(cameraIPData);
        } else {
            MyDataBase::getInstance()->altCameraIPData(cameraIPData);
        }

        // 根据情况返回不同的值
        if (res == CAMERA_STATUS_SUCCESS) {
            return 2;
        } else {
            return -2;
        }

    }

    // 数据库交互
    if (MyDataBase::getInstance()->queCameraIPData(cameraIPData).cameraId == -1) {
        MyDataBase::getInstance()->addCameraIPData(cameraIPData);
    } else {
        MyDataBase::getInstance()->altCameraIPData(cameraIPData);
    }

    for (int i = 0; i < 6; i++) {
        qDebug() << QString(ipInfo[i]);
    }

    // 获取相机序列号信息
    qDebug() << tCameraEnumList[0].acProductSeries;
    qDebug() << tCameraEnumList[0].acProductName;
    qDebug() << tCameraEnumList[0].acFriendlyName;
    qDebug() << tCameraEnumList[0].acLinkName;
    qDebug() << tCameraEnumList[0].acSn;
    qDebug() << "1";

    //相机初始化。初始化成功后，才能调用任何其他相机相关的操作接口
    iStatus = CameraInit(&tCameraEnumList[0],-1,-1,&g_hCamera);

    //初始化失败
    if(iStatus!=CAMERA_STATUS_SUCCESS){
        return -1;
    }

    qDebug() << "2";

    //获得相机的特性描述结构体。该结构体中包含了相机可设置的各种参数的范围信息。决定了相关函数的参数
    CameraGetCapability(g_hCamera,&g_tCapability);
    qDebug() << "3";

    g_pRgbBuffer = (unsigned char*)malloc(g_tCapability.sResolutionRange.iHeightMax*g_tCapability.sResolutionRange.iWidthMax*3);
    g_readBuf = (unsigned char*)malloc(g_tCapability.sResolutionRange.iHeightMax*g_tCapability.sResolutionRange.iWidthMax*3);
    qDebug() << "4";

    /*让SDK进入工作模式，开始接收来自相机发送的图像
    数据。如果当前相机是触发模式，则需要接收到
    触发帧以后才会更新图像。    */
    CameraPlay(g_hCamera);
    qDebug() << "5";

    /*
        设置图像处理的输出格式，彩色黑白都支持RGB24位
    */
    if(g_tCapability.sIspCapacity.bMonoSensor){
        CameraSetIspOutFormat(g_hCamera,CAMERA_MEDIA_TYPE_MONO8);
    }else{
        CameraSetIspOutFormat(g_hCamera,CAMERA_MEDIA_TYPE_RGB8);
    }
    qDebug() << "6";

    return 0;
}

void ImgArea::cameraStatues()
{
//    m_camera_statuesFps->setText(QString("Capture fps: %1  Display fps :%2").arg(QString::number(g_read_fps, 'f', 2)).arg(QString::number(g_disply_fps, 'f', 2)));
    g_read_fps=0;
    g_disply_fps=0;
}

int ImgArea::initParameter(int hCamera, tSdkCameraCapbility *pCameraInfo)
{
    tSdkImageResolution     *pImageSizeDesc=pCameraInfo->pImageSizeDesc;// 预设分辨率选择
    tSdkImageResolution     sResolution;  //获取当前设置到分辨率
    tSdkExpose              *SdkExpose = &pCameraInfo->sExposeDesc;

    //获得当前预览的分辨率。
    CameraGetImageResolution(hCamera,&sResolution);

    g_W_H_INFO.sensor_width=pImageSizeDesc[sResolution.iIndex].iWidth;
    g_W_H_INFO.sensor_height=pImageSizeDesc[sResolution.iIndex].iHeight;
    g_W_H_INFO.buffer_size= g_W_H_INFO.sensor_width*g_W_H_INFO.sensor_height;

    BOOL            AEstate=FALSE;

    //获得相机当前的曝光模式。
//    CameraSetAeState(hCamera, true);
    CameraGetAeState(hCamera,&AEstate);
    CameraSetAeState(hCamera, false);

    //获得相机的曝光时间。
    CameraGetExposureTime(hCamera,&m_pfExposureTime);

    //获得图像信号的模拟增益值。
    CameraGetAnalogGain(hCamera,&m_pusAnalogGain);

    //当前的行曝光时间，单位为us
    /*
        获得一行的曝光时间。对于CMOS传感器，其曝光
        的单位是按照行来计算的，因此，曝光时间并不能在微秒
        级别连续可调。而是会按照整行来取舍。这个函数的
        作用就是返回CMOS相机曝光一行对应的时间。
    */
    CameraGetExposureLineTime(hCamera, &m_expLineTime);

    m_exposureTimeMin = SdkExpose->uiExposeTimeMin;
    m_exposureTimeMax = SdkExpose->uiExposeTimeMax;

    m_analogGainMin = SdkExpose->uiAnalogGainMin;
    m_analogGainMax = SdkExpose->uiAnalogGainMax;

    return 1;
}

QString ImgArea::pointListToStr(QList<QPointF> pointList)
{
    QString resStr = "";

    for (int i = 0; i < pointList.size(); i++) {
        resStr += QString::number(pointList[i].x()) + "," + QString::number(pointList[i].y()) + ",";
    }

    return resStr;
}

Ptr<BackgroundSubtractorMOG2> ImgArea::getMOG2Data(ShapeItemData itemData)
{
    Ptr<BackgroundSubtractorMOG2> myMOG2Data;

    Mat mask,srcFg, dstBg, dstFg;
//    mask = getShapeMask(itemData, QImage(), itemData);



    return myMOG2Data;
}

void ImgArea::updateShapeImgMold(int cameraId, int sceneId)
{
    qDebug() << "in ImgArea::updateShapeImgMold";
    if (sceneId != 1 && sceneId != 2) {
        return ;
    }

    if (cameraId <= 0) {
        return ;
    }

    // 清理原数据
    if (sceneId == 1) {
        m_cameraDetectDataList[cameraId - 1].moldShapeMaskList.clear();
        m_cameraDetectDataList[cameraId - 1].moldMOG2DataList.clear();
        m_moldShapeMaskList.clear();
        m_moldMOG2DataList.clear();
    } else {
        m_cameraDetectDataList[cameraId - 1].prodShapeMaskList.clear();
        m_cameraDetectDataList[cameraId - 1].prodMOG2DataList.clear();
        m_prodShapeMaskList.clear();
        m_prodMOG2DataList.clear();
    }

    // 获取图形模板
    ShapeItemData itemData;
    itemData.cameraId = TitleBar::getInstance()->getCurCameraId();
    itemData.sceneId  = sceneId;
    itemData.moldId   = 1;

    QList<ShapeItemData> itemDataList = MyDataBase::getInstance()->queShapeItemData(itemData);

    // 获取图片模板
    ImageMoldData imgData;
    imgData.cameraId = TitleBar::getInstance()->getCurCameraId();
    imgData.sceneId  = sceneId;

    QList<ImageMoldData> imgDataList = MyDataBase::getInstance()->queAllImgMoldData(imgData);

    if (imgDataList.size() == 0) {
        return ;
    }

    emit startUpdateMoldSig(cameraId, sceneId, itemDataList, imgDataList);

    return ;

    // 获取屏蔽区
    QList<ShapeItemData> maskItemDataList;
    QList<int> maskItemIdx;
    for (int i = 0; i < itemDataList.size(); i++) {
        if (itemDataList[i].type == MyGraphicsItem::ItemType::Polygon_Mask) {
            ShapeItemData maskItemData = itemDataList[i];
            maskItemData.sceneId = -1;
            maskItemDataList.append(maskItemData);
            maskItemIdx.append(i);
        }
    }

    for (int i = maskItemIdx.size() - 1; i >= 0; i--) {
        itemDataList.removeAt(maskItemIdx[i]);
    }

    // 获取图形mask
    for (int i = 0; i < itemDataList.size(); i++) {
        QImage img = QImage(imgDataList[0].imgPath).scaled(this->size());
        Mat shapeMask = getShapeMask(itemDataList[i], img, maskItemDataList);

        if (sceneId == 1) {
            m_moldShapeMaskList.append(shapeMask);
        } else {
            m_prodShapeMaskList.append(shapeMask);
        }
    }

    // 对每张图片模板进行检测
    for (int i = 0; i < imgDataList.size(); i++) {
        QImage imgBg = QImage(imgDataList[i].imgPath).scaled(this->size());


        // 对每个图形模板进行检测
        for (int j = 0; j < itemDataList.size(); j++) {
            Mat mask, dstBg;
            if (sceneId == 1) {
                mask = m_moldShapeMaskList[j];
            } else {
                mask = m_prodShapeMaskList[j];
            }

            qimToMat(imgBg).copyTo(dstBg, mask);

            double acc  = itemDataList[j].accuracy;
            int pix     = itemDataList[j].pixel;

//            qDebug() << "1";
            Ptr<BackgroundSubtractorMOG2> m_pMOG2 = createBackgroundSubtractorMOG2(50, acc, false);
            Mat fgMaskMOG2Mat;

            MyMOG2Data myMOG2Data;
            myMOG2Data.myMOG2    = m_pMOG2;
            myMOG2Data.fgMaskMat = fgMaskMOG2Mat;
            myMOG2Data.accuracy  = acc;
            myMOG2Data.pixel     = pix;

            if (sceneId == 1) {
                m_moldMOG2DataList.append(myMOG2Data);
            } else {
                m_prodMOG2DataList.append(myMOG2Data);
            }
//            qDebug() << "2";

            Mat frameBg = dstBg.clone();
            for (int k = 0; k < m_pMOG2->getHistory(); k++) {
                m_frame = frameBg.clone();
                m_pMOG2->apply(m_frame, fgMaskMOG2Mat);
            }
//            qDebug() << "3";
        }
    }

    // 获取更新后的数据
    if (sceneId == 1) {
        m_cameraDetectDataList[cameraId - 1].moldShapeMaskList = m_moldShapeMaskList;
        m_cameraDetectDataList[cameraId - 1].moldMOG2DataList  = m_moldMOG2DataList;
    } else {
        m_cameraDetectDataList[cameraId - 1].prodShapeMaskList = m_prodShapeMaskList;
        m_cameraDetectDataList[cameraId - 1].prodMOG2DataList  = m_prodMOG2DataList;
    }
}

int ImgArea::detectCurImage(int cameraId, int sceneId, int detectTimes)
{
    QDateTime detectTime;
    QString fileName;
    QString filePath;
    QImage targetImg;
    QFile tmpFile;
    int detectRes;

    // 获取当前检测相机id
    m_detectCameraId = cameraId;

    // 获取当前检测场景id
    if (sceneId == -1) {
        m_detectSceneId = SideBar::getInstance()->getCurSceneID();
    } else {
        m_detectSceneId = sceneId;
    }

    // 检测次数
    for (int t = 0; t < detectTimes; t++) {
        detectTime = QDateTime::currentDateTime();
        fileName   = detectTime.toString("yyyy-MM-dd-HH-mm-ss");
        filePath   = QString("%1/%2.png").arg(MyDataBase::dbFilePath).arg(fileName);
        m_curDetectImage = getCurImage();
        m_curDetectImage.save(filePath);

        m_isShowImage = false;
        loadImage(filePath);

        targetImg = QImage(filePath);
//        targetImg = getCurImage();

        // 删除临时文件
        tmpFile.setFileName(fileName);
        tmpFile.remove();

        // 获得检测结果
        detectRes = detectImage(targetImg, m_detectCameraId, m_detectSceneId);

        // 检测到NG
        if (detectRes == DetectRes::NG) {
            if (t != detectTimes - 1) {
                continue ;
            }

            // 显示检测结果
            setDetectRes(false, m_detectSceneId);
            drawDetectResult(m_resPointList);

            // 指示灯变化
            if (m_detectSceneId == 1) {
                SideBar::getInstance()->setCanClampMoldState(RadioBtnState::Wrong);
            } else {
                SideBar::getInstance()->setCanThimbleState(RadioBtnState::Wrong);
            }

            // 顶栏变化
            TitleBar::getInstance()->setAlarmBtnState(true);

        // 检测到OK
        } else if (detectRes == DetectRes::OK) {

            // 显示检测结果
            setDetectRes(true, m_detectSceneId);

            // 指示灯变化
            if (m_detectSceneId == 1) {
                SideBar::getInstance()->setCanClampMoldState(RadioBtnState::Correct);
            } else {
                SideBar::getInstance()->setCanThimbleState(RadioBtnState::Correct);
            }

            // 顶栏变化
            TitleBar::getInstance()->setAlarmBtnState(false);

            break;
        }
    }

    if (detectRes == DetectRes::OK) {
        m_isShowImage = true;
    } else {
        QApplication::beep();
    }

    return detectRes;
}

int ImgArea::autoDetectImage(int cameraId, int sceneId, double delayTime, int reDetectTimes)
{
    // 设置场景和延时
    m_detectCameraId = cameraId;
    m_detectSceneId  = sceneId;
    m_delayTime      = delayTime;
    m_reDetectTimes  = reDetectTimes;

    // 显示倒计时
    m_pSigTimeLab->show();
    m_sigDelayTimer->start(100);

    return 1;
}

int ImgArea::detectImage(QImage imgFg, int cameraId, int sceneId)
{
    int detectRes = DetectRes::NG;

    imgFg = imgFg.scaled(this->size());

    // 获取图形模板
    ShapeItemData itemData;
    itemData.cameraId = cameraId;
    itemData.sceneId  = sceneId;
    itemData.moldId   = 1;

    QList<ShapeItemData> itemDataList = MyDataBase::getInstance()->queShapeItemData(itemData);

    // 获取图片模板
    ImageMoldData imgData;
    imgData.cameraId = cameraId;
    imgData.sceneId  = sceneId;

    QList<ImageMoldData> imgDataList = MyDataBase::getInstance()->queAllImgMoldData(imgData);

    if (itemDataList.size() == 0 || imgDataList.size() == 0) {
        return DetectRes::OK;
    }

//    // 获取屏蔽区
//    QList<ShapeItemData> maskItemDataList;
//    QList<int> maskItemIdx;
//    for (int i = 0; i < itemDataList.size(); i++) {
//        if (itemDataList[i].type == MyGraphicsItem::ItemType::Polygon_Mask) {
//            ShapeItemData maskItemData = itemDataList[i];
//            maskItemData.sceneId = -1;
//            maskItemDataList.append(maskItemData);
//            maskItemIdx.append(i);
//        }
//    }

//    for (int i = maskItemIdx.size() - 1; i >= 0; i--) {
//        itemDataList.removeAt(maskItemIdx[i]);
//    }

    // 获取图形mask
//    m_shapeMaskList.clear();

//    for (int i = 0; i < itemDataList.size(); i++) {
//        Mat shapeMask = getShapeMask(itemDataList[i], imgFg, maskItemDataList);
//        m_shapeMaskList.append(shapeMask);
//    }

    emit startDetectImageSig(imgFg, cameraId, sceneId, detectRes);
    return detectRes;

//    if (detectSceneId == 1) {
//        m_moldShapeMaskList = m_cameraDetectDataList[cameraId - 1].moldShapeMaskList;
//        m_moldMOG2DataList  = m_cameraDetectDataList[cameraId - 1].moldMOG2DataList;
//    } else {
//        m_prodShapeMaskList = m_cameraDetectDataList[cameraId - 1].prodShapeMaskList;
//        m_prodMOG2DataList  = m_cameraDetectDataList[cameraId - 1].prodMOG2DataList;
//    }

//    QVector<QVector<QPointF>> resPointList;

//    // 对每张图片模板进行检测
//    for (int i = 0; i < imgDataList.size(); i++) {

//        int imgMoldDeteRes = DetectRes::OK;

//        int shapeMaskSize = (detectSceneId == 1) ? m_moldShapeMaskList.size() : m_prodShapeMaskList.size();

//        // 对每个图形模板进行检测
//        for (int j = 0; j < shapeMaskSize; j++) {
//            // 获取图形mask
//            Mat mask, srcFg, dstFg;
//            if (detectSceneId == 1) {
//                mask = m_moldShapeMaskList[j];
//            } else {
//                mask = m_prodShapeMaskList[j];
//            }

//            // 获取MOG2数据
//            MyMOG2Data myMOG2Data;
//            if (detectSceneId == 1) {
//                myMOG2Data = m_moldMOG2DataList[i * shapeMaskSize + j];
//            } else {
//                myMOG2Data = m_prodMOG2DataList[i * shapeMaskSize + j];
//            }

//            srcFg = qimToMat(imgFg);
//            qimToMat(imgFg).copyTo(dstFg, mask);

//            int pix = myMOG2Data.pixel;

//            Mat frameFg = dstFg.clone();

////            for (int k = 0; k < myMOG2Data.myMOG2->getHistory(); k++) {
////                m_frame = frameBg.clone();
////                myMOG2Data.myMOG2->apply(m_frame, myMOG2Data.fgMaskMat);
////            }

//            Mat m_frame = frameFg.clone();
//            myMOG2Data.myMOG2->apply(m_frame, myMOG2Data.fgMaskMat, 0);

//    //        rectangle(m_frame, cv::Point(10, 2), cv::Point(100, 20),
//    //                  cv::Scalar(255, 255, 255), -1);

//            // get result
//            vector<vector<Point>> contours;
//            vector<Vec4i> hierarchy;
//            findContours(myMOG2Data.fgMaskMat, contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());
//            Mat imageContours = Mat::zeros(m_fgMaskMOG2.size(), CV_8UC1);
//            Mat contoursMat = Mat::zeros(m_fgMaskMOG2.size(), CV_8UC1);
//            vector<vector<Point>> approxPoint(contours.size());

//            for (int k = 0; k < int(contours.size()); k++) {
//                if (contourArea(contours[k]) > pix) {

//                    qDebug() << "contourArea(contours[k]):" << contourArea(contours[k]);

//                    // 绘制轮廓
//                    drawContours(srcFg, contours, k, Scalar(0, 0, 255), 2, 8, hierarchy);

//                    // 出现NG
//                    imgMoldDeteRes = DetectRes::NG;

//                    if (i == imgDataList.size() - 1) {
//                        QVector<QPointF> singlePointList;
//                        for (int m = 0; m < int(contours[k].size()); m++) {
//                            singlePointList.append(QPointF(contours[k][m].x, contours[k][m].y));
//                        }
//                        resPointList.append(singlePointList);
//                    }
//                }
//            }

//            imshow(QString("mask_%1_%2").arg(i).arg(j).toStdString(), mask);
//            imshow(QString("fgMaskMOG2_%1_%2").arg(i).arg(j).toStdString(), myMOG2Data.fgMaskMat);
////            imshow(QString("detect result_%1_%2").arg(i).arg(j).toStdString(), srcFg);

////            myMOG2Data.myMOG2->clear();
//        }

//        if (imgMoldDeteRes == DetectRes::OK) {
//            detectRes = DetectRes::OK;
//            break;
//        }
//    }

//    if (detectRes == DetectRes::NG) {
//        loadShapeItem(itemData);
//        drawDetectResult(resPointList);
//    }

//    qDebug() << "detect res: " << detectRes;
//    return detectRes;
}

void ImgArea::getDetectResult(int result, QVector<QVector<QPointF> > resPointList, QList<int> resAreaSizeList)
{
    qDebug() << "detect res: " << result;

    // NG区域的边缘点
    m_resPointList = resPointList;

    // NG区域的范围大小
    m_resAreaSizeList = resAreaSizeList;

    return ;
}

QImage ImgArea::getCurDetectImage()
{
    return m_curDetectImage;
}

int ImgArea::getCurDetectCameraId()
{
    return m_detectCameraId;
}

int ImgArea::getCurDetectSceneId()
{
    return m_detectSceneId;
}

int ImgArea::getShapeItemNum()
{
    int count = 0;
    for (auto &temp : m_pScene->items()) {
        if (temp != m_pImageItem) {
            count++;
        }
    }
    return count;
}

Mat ImgArea::getShapeMask(ShapeItemData itemData, QImage img, QList<ShapeItemData> maskItemDataList)
{
    cv::Mat mask, dst;
//    QImage img = m_pImageItem->pixmap().toImage();

    img = img.scaled(this->size());

    qDebug() << "img size: " << img.width() << " " << img.height();
    qimToMat(img).copyTo(mask);
//    mask = cv::Mat(this->height(), this->width(), CV_8UC4, cv::Scalar(0));
    mask.setTo(cv::Scalar::all(0));


    QStringList centerList = itemData.center.split(',');
    QPointF center;
    if (centerList.size() >= 2) {
        center = QPointF(centerList[0].toInt(), centerList[1].toInt());
    }

    switch (itemData.type) {
    case MyGraphicsItem::ItemType::Rectangle: {
        QStringList edgeList = itemData.edge.split(',');
        QPointF edge;
        if (edgeList.size() >= 2) {
            edge = QPointF(edgeList[0].toInt(), edgeList[1].toInt());
        }

        cv::rectangle(mask, Point(center.x() - edge.x()/2, center.y() - edge.y()/2),
                      Point(center.x() + edge.x()/2, center.y() + edge.y()/2),
                      Scalar(255, 255, 255), -1, 8, 0);
    }
        break;
    case MyGraphicsItem::ItemType::Polygon: {
        QStringList myStrList = itemData.pointList.split(',');
        vector<vector<Point>> myEdgePointList;
        vector<Point> myEdgePoint;

        if (myStrList.size() > 2) {
            for (int i = 0; i < myStrList.size() - 1; i+=2) {
                if (i + 1 < myStrList.size()) {
                    myEdgePoint.push_back(Point(myStrList[i].toInt(), myStrList[i+1].toInt()));
                }
            }
            myEdgePointList.push_back(myEdgePoint);
        }
        cv::fillPoly(mask, myEdgePointList, Scalar(255, 255, 255));

    }
        break;
    case MyGraphicsItem::ItemType::Curve: {
        QStringList myStrList = itemData.pointList.split(',');
        vector<vector<Point>> myEdgePointList;
        vector<Point> myEdgePoint;

        if (myStrList.size() > 2) {
            for (int i = 0; i < myStrList.size() - 1; i+=2) {
                if (i + 1 < myStrList.size()) {
                    myEdgePoint.push_back(Point(myStrList[i].toInt(), myStrList[i+1].toInt()));
                }
            }
            myEdgePointList.push_back(myEdgePoint);
        }
        cv::fillPoly(mask, myEdgePointList, Scalar(255, 255, 255));
    }
        break;
    case MyGraphicsItem::ItemType::Circle: {
        QStringList edgeList = itemData.edge.split(',');
        QPointF edge;
        if (edgeList.size() >= 2) {
            edge = QPointF(edgeList[0].toInt(), edgeList[1].toInt());
        }
        cv::circle(mask, Point(center.x(), center.y()), edge.x(), Scalar(255, 255, 255), -1, 8, 0);
    }
        break;
    case MyGraphicsItem::ItemType::Concentric_Circle: {
        QStringList edgeList = itemData.edge.split(',');
        QPointF edge;
        if (edgeList.size() >= 2) {
            edge = QPointF(edgeList[0].toInt(), edgeList[1].toInt());
        }

        int minRadius = edge.x() > edge.y() ? edge.y() : edge.x();
        int maxRadius = edge.x() > edge.y() ? edge.x() : edge.y();
        cv::circle(mask, Point(center.x(), center.y()), maxRadius, Scalar(255, 255, 255), -1, 8, 0);
        cv::circle(mask, Point(center.x(), center.y()), minRadius, Scalar(0, 0, 0), -1, 8, 0);
    }
        break;
    default:
        break;
    }

    for (int i = 0; i < maskItemDataList.size(); i++) {
        ShapeItemData maskItemData = maskItemDataList[i];
        QStringList maskStrList = maskItemData.pointList.split(',');
        vector<vector<Point>> maskEdgePointList;
        vector<Point> maskEdgePoint;

        if (maskStrList.size() > 2) {
            for (int i = 0; i < maskStrList.size() - 1; i+=2) {
                if (i + 1 < maskStrList.size()) {
                    maskEdgePoint.push_back(Point(maskStrList[i].toInt(), maskStrList[i+1].toInt()));
                }
            }
            maskEdgePointList.push_back(maskEdgePoint);
        }
        cv::fillPoly(mask, maskEdgePointList, Scalar(0, 0, 0));
    }

//    imshow("single mask", mask);
    return mask;
}

void ImgArea::setShowState(bool isShow)
{
    m_isShowImage = isShow;

    if (m_isShowImage == false) {
        m_pImageItem->hide();
    } else {
        m_pImageItem->show();
    }
}

bool ImgArea::getShowState()
{
    return m_isShowImage;
}

QImage ImgArea::getCurImage()
{
    return m_pCurImage;
}

int ImgArea::getCameraCounts()
{
    return m_cameraCounts;
}

int ImgArea::getCameraStatus(int cameraId)
{
    Q_UNUSED(cameraId);

    return status;
}

void ImgArea::drawDetectResult(QVector<QVector<QPointF>> resPointList)
{
    QPen polyPen = QPen(Qt::red, 2);
    qDebug() << "m_resPointList.size(): " << m_resPointList.size();
    for (int i = 0; i < m_resPointList.size(); i++) {

        // 绘制边缘范围
        QPolygonF resPoly = QPolygonF(m_resPointList[i]);
        m_detectResItemList.append(m_pScene->addPolygon(resPoly, polyPen));

        // 显示范围大小
        QFont textFont = this->font();
        textFont.setPixelSize(20);

        QGraphicsTextItem *textItem = new QGraphicsTextItem();
        textItem->setPos(QPointF(m_resPointList[i][0].x(), m_resPointList[i][0].y()));
        textItem->setPlainText(QString::number(m_resAreaSizeList[i]));
        textItem->setFont(textFont);
        textItem->setDefaultTextColor(Qt::red);
        m_pScene->addItem(textItem);
        m_detectResTxtItemList.append(textItem);
    }
}

void ImgArea::clearDetectResult()
{
    for (int i = 0; i < m_detectResItemList.size(); i++) {
        m_pScene->removeItem(m_detectResItemList[i]);
        m_pScene->removeItem(m_detectResTxtItemList[i]);
    }

    m_detectResItemList.clear();
    m_detectResTxtItemList.clear();
    m_pResultLab->hide();

//    clearShapes();
}

void ImgArea::setShapeNoMove(bool noMove)
{
    m_pView->setAttribute(Qt::WA_TransparentForMouseEvents, noMove);
}

int ImgArea::getDetectSceneId()
{
    return m_detectSceneId;
}

void ImgArea::setSceneDelayTime(int sceneId, double delayTime)
{
    // 设置场景和延时
    m_detectSceneId = sceneId;
    m_delayTime = delayTime;

    // 显示倒计时
    m_pSigTimeLab->show();
    m_sigDelayTimer->start(100);
}

int ImgArea::initLocalNetwork()
{
    QList<QNetworkInterface> ifaceList = QNetworkInterface::allInterfaces();
    qDebug() << "ifaceList.size(): " << ifaceList.size();

    QProcess cmd(this);
    QString cmdStr = QString("netsh interface ipv4 set address name=\"%1\" source=static "
                             "address=%2 mask=255.255.255.0 gateway=%3");

    for (int i = 0; i < ifaceList.size(); i++) {

        QNetworkInterface iface = ifaceList[i];

        if (!iface.isValid())
            continue;

        QNetworkInterface::InterfaceFlags flags = iface.flags();

        // 网络接口处于活动状态 && 不是本地回环地址
        if (flags.testFlag(QNetworkInterface::IsRunning)
        && !flags.testFlag(QNetworkInterface::IsLoopBack)) {
            qDebug() << QString("设备 %1").arg(i);
            qDebug() << "接口名称: " << iface.humanReadableName();
            qDebug() << "设备名称: " << iface.name();
            qDebug() << "硬件地址: " << iface.hardwareAddress();

            qDebug() << "IP地址列表: ";

            // 读取IP地址的信息列表
            QList<QNetworkAddressEntry> entryList = iface.addressEntries();

            for (int j = 0; j < entryList.size(); j++) {
                QNetworkAddressEntry entry = entryList[j];

                // 判断是否为IPV4地址
                if (entry.ip().protocol() != QAbstractSocket::IPv4Protocol) {
                    continue ;
                }

                qDebug() << QString("地址 %1").arg(j);
                qDebug() << "IP地址: " << entry.ip().toString();
                qDebug() << "子网掩码: " << entry.netmask().toString();
                qDebug() << "广播地址: " << entry.broadcast().toString();

                // 修改本机IP地址
                int ifaceId = i + 1;

                // 测试
//                ifaceId = 2;

                QString localFaceIp = m_ifaceIp.arg(ifaceId).arg(ifaceId);
                QString gateway     = m_gateway.arg(ifaceId);
                if (entry.ip().toString() != localFaceIp) {
                    cmd.start(cmdStr.arg(iface.name()).arg(localFaceIp).arg(gateway));
                    cmd.waitForFinished();
                }
            }
        }
    }

    return 1;
}

QImage ImgArea::matToQim(Mat &mat)
{
    cvtColor(mat, mat, COLOR_BGR2RGB);
    QImage qim((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step,
               QImage::Format_RGB888);
    return qim;
}

Mat ImgArea::qimToMat(QImage &qim)
{
    Mat mat = Mat(qim.height(), qim.width(),
                  CV_8UC4,(void*)qim.constBits(),qim.bytesPerLine());
    return mat;
}

void ImgArea::setSigDelayTimeLab()
{
    m_delayTime = m_delayTime - 100;

    QString sceneName = m_detectSceneId == 1 ? "检模" : "产品";
    QString delayText = QString("%1信号: %2").arg(sceneName).arg(m_delayTime / 1000);

    m_pSigTimeLab->clear();
    m_pSigTimeLab->setText(delayText);

    if (m_delayTime <= 110) {
        m_pSigTimeLab->clear();
        m_pSigTimeLab->hide();
    }
    if (m_delayTime <= 10) {
        m_sigDelayTimer->stop();

        setShapeNoMove(true);
        clearDetectResult();
        detectCurImage(m_detectCameraId, m_detectSceneId, m_reDetectTimes);
    }
}

void ImgArea::imageProcess(QImage img)
{
    if (m_thread->quit) {
        return ;
    }

//    if(m_pImageItem) {
//        m_pScene->removeItem(m_pImageItem);
//        delete m_pImageItem;
//        m_pImageItem = 0;
//    }

//    m_pImageItem = m_pScene->addPixmap((QPixmap::fromImage(img)).scaled(this->size()));

//    //获得相机的曝光时间。
//    CameraGetExposureTime(g_hCamera,&m_pfExposureTime);

//    //获得图像信号的模拟增益值。
//    CameraGetAnalogGain(g_hCamera,&m_pusAnalogGain);

//    m_pScene->setSceneRect(0, 0, img.width(), img.height());

    m_pCurImage = img;

    if (m_isShowImage) {
        QPixmap imgPix = (QPixmap::fromImage(img));
        imgPix = imgPix.scaled(this->size());
        m_pImageItem->setPixmap(imgPix);
    }

    g_disply_fps++;
}


DetectImageWork::DetectImageWork(QObject *parent)
    : QObject(parent)
{

}

DetectImageWork::~DetectImageWork()
{

}

void DetectImageWork::setCameraCounts(int cameraCounts)
{
    for (int i = 0; i < cameraCounts; i++) {
        CameraDetectData cameraDetectData;
        cameraDetectData.cameraId = i + 1;

        m_cameraDetectDataList.append(cameraDetectData);
    }
}

void DetectImageWork::setSceneRectSize(QSize size)
{
    qDebug() << "setSceneRectSize";
    m_sceneRectSize = size;
}

Mat DetectImageWork::getShapeMask(ShapeItemData itemData, QImage img, QList<ShapeItemData> maskItemDataList)
{
    cv::Mat mask;
//    QImage img = m_pImageItem->pixmap().toImage();

    img = img.scaled(m_sceneRectSize);

    qDebug() << "img size: " << img.width() << " " << img.height();
    qimToMat(img).copyTo(mask);
//    mask = cv::Mat(this->height(), this->width(), CV_8UC4, cv::Scalar(0));
    mask.setTo(cv::Scalar::all(0));

//    std::string base64Data = Mat2Base64(qimToMat(img), QString("png").toStdString());
//    qDebug() << "base64Data.size: " << base64Data.size();
//    qDebug() << base64Data[0] << base64Data[1];

    QStringList centerList = itemData.center.split(',');
    QPointF center;
    if (centerList.size() >= 2) {
        center = QPointF(centerList[0].toInt(), centerList[1].toInt());
    }

    switch (itemData.type) {
    case MyGraphicsItem::ItemType::Rectangle: {
        QStringList edgeList = itemData.edge.split(',');
        QPointF edge;
        if (edgeList.size() >= 2) {
            edge = QPointF(edgeList[0].toInt(), edgeList[1].toInt());
        }

        cv::rectangle(mask, Point(center.x() - edge.x()/2, center.y() - edge.y()/2),
                      Point(center.x() + edge.x()/2, center.y() + edge.y()/2),
                      Scalar(255, 255, 255), -1, 8, 0);
    }
        break;
    case MyGraphicsItem::ItemType::Polygon: {
        QStringList myStrList = itemData.pointList.split(',');
        vector<vector<Point>> myEdgePointList;
        vector<Point> myEdgePoint;

        if (myStrList.size() > 2) {
            for (int i = 0; i < myStrList.size() - 1; i+=2) {
                if (i + 1 < myStrList.size()) {
                    myEdgePoint.push_back(Point(myStrList[i].toInt(), myStrList[i+1].toInt()));
                }
            }
            myEdgePointList.push_back(myEdgePoint);
        }
        cv::fillPoly(mask, myEdgePointList, Scalar(255, 255, 255));

    }
        break;
    case MyGraphicsItem::ItemType::Curve: {
        QStringList myStrList = itemData.pointList.split(',');
        vector<vector<Point>> myEdgePointList;
        vector<Point> myEdgePoint;

        if (myStrList.size() > 2) {
            for (int i = 0; i < myStrList.size() - 1; i+=2) {
                if (i + 1 < myStrList.size()) {
                    myEdgePoint.push_back(Point(myStrList[i].toInt(), myStrList[i+1].toInt()));
                }
            }
            myEdgePointList.push_back(myEdgePoint);
        }
        cv::fillPoly(mask, myEdgePointList, Scalar(255, 255, 255));
    }
        break;
    case MyGraphicsItem::ItemType::Circle: {
        QStringList edgeList = itemData.edge.split(',');
        QPointF edge;
        if (edgeList.size() >= 2) {
            edge = QPointF(edgeList[0].toInt(), edgeList[1].toInt());
        }
        cv::circle(mask, Point(center.x(), center.y()), edge.x(), Scalar(255, 255, 255), -1, 8, 0);
    }
        break;
    case MyGraphicsItem::ItemType::Concentric_Circle: {
        QStringList edgeList = itemData.edge.split(',');
        QPointF edge;
        if (edgeList.size() >= 2) {
            edge = QPointF(edgeList[0].toInt(), edgeList[1].toInt());
        }

        int minRadius = edge.x() > edge.y() ? edge.y() : edge.x();
        int maxRadius = edge.x() > edge.y() ? edge.x() : edge.y();
        cv::circle(mask, Point(center.x(), center.y()), maxRadius, Scalar(255, 255, 255), -1, 8, 0);
        cv::circle(mask, Point(center.x(), center.y()), minRadius, Scalar(0, 0, 0), -1, 8, 0);
    }
        break;
    default:
        break;
    }

    for (int i = 0; i < maskItemDataList.size(); i++) {
        ShapeItemData maskItemData = maskItemDataList[i];
        QStringList maskStrList = maskItemData.pointList.split(',');
        vector<vector<Point>> maskEdgePointList;
        vector<Point> maskEdgePoint;

        if (maskStrList.size() > 2) {
            for (int i = 0; i < maskStrList.size() - 1; i+=2) {
                if (i + 1 < maskStrList.size()) {
                    maskEdgePoint.push_back(Point(maskStrList[i].toInt(), maskStrList[i+1].toInt()));
                }
            }
            maskEdgePointList.push_back(maskEdgePoint);
        }
        cv::fillPoly(mask, maskEdgePointList, Scalar(0, 0, 0));
    }

//    imshow("single mask", mask);
    return mask;
}

void DetectImageWork::updateShapeImgMold(int cameraId, int sceneId, QList<ShapeItemData> itemDataList, QList<ImageMoldData> imgDataList)
{
    qDebug() << "in DetectImageWork updateShapeImgMold";
    if (sceneId != 1 && sceneId != 2) {
        return ;
    }

    if (cameraId <= 0) {
        return ;
    }

    // 清理原数据
    if (sceneId == 1) {
        m_cameraDetectDataList[cameraId - 1].moldShapeMaskList.clear();
        m_cameraDetectDataList[cameraId - 1].moldMOG2DataList.clear();
        m_moldShapeMaskList.clear();
        m_moldMOG2DataList.clear();
    } else {
        m_cameraDetectDataList[cameraId - 1].prodShapeMaskList.clear();
        m_cameraDetectDataList[cameraId - 1].prodMOG2DataList.clear();
        m_prodShapeMaskList.clear();
        m_prodMOG2DataList.clear();
    }

//    // 获取图形模板
//    ShapeItemData itemData;
//    itemData.cameraId = cameraId;
//    itemData.sceneId  = sceneId;
//    itemData.moldId   = 1;

//    QList<ShapeItemData> itemDataList = MyDataBase::getInstance()->queShapeItemData(itemData);

//    // 获取图片模板
//    ImageMoldData imgData;
//    imgData.cameraId = cameraId;
//    imgData.sceneId  = sceneId;

//    QList<ImageMoldData> imgDataList = MyDataBase::getInstance()->queAllImgMoldData(imgData);

    if (imgDataList.size() == 0) {
        return ;
    }

    // 获取屏蔽区
    QList<ShapeItemData> maskItemDataList;
    QList<int> maskItemIdx;
    for (int i = 0; i < itemDataList.size(); i++) {
        if (itemDataList[i].type == MyGraphicsItem::ItemType::Polygon_Mask) {
            ShapeItemData maskItemData = itemDataList[i];
            maskItemData.sceneId = -1;
            maskItemDataList.append(maskItemData);
            maskItemIdx.append(i);
        }
    }

    for (int i = maskItemIdx.size() - 1; i >= 0; i--) {
        itemDataList.removeAt(maskItemIdx[i]);
    }

    // 获取图形mask
    for (int i = 0; i < itemDataList.size(); i++) {
        QImage img = QImage(imgDataList[0].imgPath).scaled(m_sceneRectSize);
        Mat shapeMask = getShapeMask(itemDataList[i], img, maskItemDataList);

        if (sceneId == 1) {
            m_moldShapeMaskList.append(shapeMask);
        } else {
            m_prodShapeMaskList.append(shapeMask);
        }
    }

    // 对每张图片模板进行检测
    for (int i = 0; i < imgDataList.size(); i++) {
        QImage imgBg = QImage(imgDataList[i].imgPath).scaled(m_sceneRectSize);

        // 对每个图形模板进行检测
        for (int j = 0; j < itemDataList.size(); j++) {
            Mat mask, dstBg;
            if (sceneId == 1) {
                mask = m_moldShapeMaskList[j];
            } else {
                mask = m_prodShapeMaskList[j];
            }

            qimToMat(imgBg).copyTo(dstBg, mask);

            double acc  = itemDataList[j].accuracy;
            int pix     = itemDataList[j].pixel;

//            qDebug() << "1";
            Ptr<BackgroundSubtractorMOG2> m_pMOG2 = createBackgroundSubtractorMOG2(50, acc, false);
            Mat fgMaskMOG2Mat;

//            qDebug() << "2";

            Mat frameBg = dstBg.clone();
            for (int k = 0; k < m_pMOG2->getHistory(); k++) {
                m_frame = frameBg.clone();
                m_pMOG2->apply(m_frame, fgMaskMOG2Mat);
            }
//            qDebug() << "3";

            MyMOG2Data myMOG2Data;
            myMOG2Data.myMOG2    = m_pMOG2;
            myMOG2Data.fgMaskMat = fgMaskMOG2Mat.clone();
            myMOG2Data.accuracy  = acc;
            myMOG2Data.pixel     = pix;

            if (sceneId == 1) {
                m_moldMOG2DataList.append(myMOG2Data);
            } else {
                m_prodMOG2DataList.append(myMOG2Data);
            }

//            imshow(QString("fgMaskMOG2_%1_%2").arg(i).arg(j).toStdString(), myMOG2Data.fgMaskMat);
        }
    }

    // 获取更新后的数据
    if (sceneId == 1) {
        m_cameraDetectDataList[cameraId - 1].moldShapeMaskList = m_moldShapeMaskList;
        m_cameraDetectDataList[cameraId - 1].moldMOG2DataList  = m_moldMOG2DataList;
    } else {
        m_cameraDetectDataList[cameraId - 1].prodShapeMaskList = m_prodShapeMaskList;
        m_cameraDetectDataList[cameraId - 1].prodMOG2DataList  = m_prodMOG2DataList;
    }

    qDebug() << "finish updateShapeImgMold";
    return ;
}

void DetectImageWork::detectImage(QImage imgFg, int cameraId, int sceneId, int &detectRes)
{
    detectRes = DetectRes::NG;

    imgFg = imgFg.scaled(m_sceneRectSize);

    if (sceneId == 1) {
        m_moldShapeMaskList = m_cameraDetectDataList[cameraId - 1].moldShapeMaskList;
        m_moldMOG2DataList  = m_cameraDetectDataList[cameraId - 1].moldMOG2DataList;
    } else {
        m_prodShapeMaskList = m_cameraDetectDataList[cameraId - 1].prodShapeMaskList;
        m_prodMOG2DataList  = m_cameraDetectDataList[cameraId - 1].prodMOG2DataList;
    }

    QVector<QVector<QPointF>> resPointList;
    QList<int> resAreaSizeList;
    Mat mask, srcFg;


    int shapeMaskSize = (sceneId == 1) ? m_moldShapeMaskList.size() : m_prodShapeMaskList.size();
    int mog2DataSize  = (sceneId == 1) ? m_moldMOG2DataList.size()  : m_prodMOG2DataList.size();
    int imgDataSize   = mog2DataSize / shapeMaskSize;

    // 对每张图片模板进行检测
    for (int i = 0; i < imgDataSize; i++) {

        int imgMoldDeteRes = DetectRes::OK;

        // 对每个图形模板进行检测
        for (int j = 0; j < shapeMaskSize; j++) {

            // 获取图形mask
            if (sceneId == 1) {
                mask = m_moldShapeMaskList[j];
            } else {
                mask = m_prodShapeMaskList[j];
            }

            // 获取MOG2数据
            MyMOG2Data myMOG2Data;
            if (sceneId == 1) {
                myMOG2Data = m_moldMOG2DataList[i * shapeMaskSize + j];
            } else {
                myMOG2Data = m_prodMOG2DataList[i * shapeMaskSize + j];
            }

            srcFg = qimToMat(imgFg);
            Mat dstFg;
            qimToMat(imgFg).copyTo(dstFg, mask);

            int pix = myMOG2Data.pixel;

            myMOG2Data.myMOG2->clear();
            myMOG2Data.myMOG2->apply(dstFg, myMOG2Data.fgMaskMat, 0);

            // get result
            vector<vector<Point>> contours;
            vector<Vec4i> hierarchy;
            findContours(myMOG2Data.fgMaskMat, contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());
//            Mat imageContours = Mat::zeros(m_fgMaskMOG2.size(), CV_8UC1);
//            Mat contoursMat = Mat::zeros(m_fgMaskMOG2.size(), CV_8UC1);
//            vector<vector<Point>> approxPoint(contours.size());

            for (int k = 0; k < int(contours.size()); k++) {
                if (contourArea(contours[k]) > pix) {

                    qDebug() << "contourArea(contours):" << contourArea(contours[k]);

                    // 绘制轮廓
//                    drawContours(srcFg, contours, k, Scalar(0, 0, 255), 2, 8, hierarchy);

                    // 出现NG
                    imgMoldDeteRes = DetectRes::NG;

                    if (i == imgDataSize - 1) {
                        QVector<QPointF> singlePointList;
                        for (int m = 0; m < int(contours[k].size()); m++) {
                            singlePointList.append(QPointF(contours[k][m].x, contours[k][m].y));
                        }
                        resPointList.append(singlePointList);
                        resAreaSizeList.append(contourArea(contours[k]));
                    }
                }
            }

//            imshow(QString("mask_%1_%2").arg(i).arg(j).toStdString(), mask);
            imshow(QString("fgMaskMat_%1_%2").arg(i).arg(j).toStdString(), myMOG2Data.fgMaskMat);
//            imshow(QString("detect result_%1_%2").arg(i).arg(j).toStdString(), srcFg);

//            myMOG2Data.myMOG2->clear();
        }

        if (imgMoldDeteRes == DetectRes::OK) {
            detectRes = DetectRes::OK;
            break;
        }
    }

    qDebug() << "finish detect";
    emit resultReadySig(detectRes, resPointList, resAreaSizeList);

    return ;
}

QImage DetectImageWork::matToQim(Mat &mat)
{
    cvtColor(mat, mat, COLOR_BGR2RGB);
    QImage qim((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step,
               QImage::Format_RGB888);
    return qim;
}

Mat DetectImageWork::qimToMat(QImage &qim)
{
    Mat mat = Mat(qim.height(), qim.width(),
                  CV_8UC4,(void*)qim.constBits(),qim.bytesPerLine());
    return mat;
}


static std::string base64Decode(const char* Data, int DataByte) {
    //解码表
    const char DecodeTable[] =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        62, // '+'
        0, 0, 0,
        63, // '/'
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
        0, 0, 0, 0, 0, 0, 0,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
        13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
        0, 0, 0, 0, 0, 0,
        26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
        39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
    };
    std::string strDecode;
    int nValue;
    int i = 0;
    while (i < DataByte) {
        if (*Data != '\r' && *Data != '\n') {
            nValue = DecodeTable[*Data++] << 18;
            nValue += DecodeTable[*Data++] << 12;
            strDecode += (nValue & 0x00FF0000) >> 16;
            if (*Data != '=') {
                nValue += DecodeTable[*Data++] << 6;
                strDecode += (nValue & 0x0000FF00) >> 8;
                if (*Data != '=') {
                    nValue += DecodeTable[*Data++];
                    strDecode += nValue & 0x000000FF;
                }
            }
            i += 4;
        }
        else {
            Data++;
            i++;
        }
    }
    return strDecode;
}


