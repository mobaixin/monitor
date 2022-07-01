#include <QMessageBox>
#include <QPixmap>
#include <QDebug>
#include <QApplication>
#include <QList>
#include <QDateTime>

#include "src/view/imgarea/imgarea.h"
#include "src/view/mainwindow.h"
#include "src/database/mydatabase.h"
#include "src/view/sidebar/sidebar.h"
#include "src/view/bottombar/bottombar.h"

#ifdef _WIN64
#pragma comment(lib,"../camera/MVCAMSDK_X64.lib")
#else
#pragma comment(lib,"../../camera/MVCAMSDK.lib")
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
    delete  m_pMainImg;

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
}

void ImgArea::setWidgetUi()
{
    m_pMainImg = new QImage;

    m_pImgAreaLayout = new QHBoxLayout(this);

    m_pView  = new QGraphicsView();
    m_pScene = new MyGraphicsScene();
    m_pImageItem = new QGraphicsPixmapItem();

    m_timer  = new QTimer(this);
    m_thread = new CaptureThread(this);

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

    m_pTipLab = new QLabel(this);
    m_pSampleLab = new QLabel(this);
    m_pResultLab = new QLabel(this);

    m_pSampleLab->hide();
    m_pResultLab->hide();

    connect(m_thread, SIGNAL(captured(QImage)),
            this, SLOT(imageProcess(QImage)), Qt::BlockingQueuedConnection);
}

void ImgArea::setWidgetStyle()
{
    m_pTipLab->setFixedSize(200, 30);
    m_pTipLab->setGeometry(10, 0, m_pTipLab->width(), m_pTipLab->height());

    m_pSampleLab->setFixedSize(500, 30);
    m_pSampleLab->setGeometry(700, 20, m_pTipLab->width(), m_pTipLab->height());

    m_pResultLab->setFixedSize(200, 30);
    m_pResultLab->setGeometry(1000, 20, m_pResultLab->width(), m_pResultLab->height());

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

    if(initSDK()==-1){
        status =0;
    } else {
        initParameter(g_hCamera,&g_tCapability);
//        CameraSetOnceWB(g_hCamera);

        m_thread->start();
        m_thread->stream();
        status = 1;
    }
    qDebug() << "status: " << status;
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
}

void ImgArea::eraseShape()
{
    if (!m_pScene->selectedItems().isEmpty()) {
        QGraphicsItem *temp = m_pScene->selectedItems().first();
        m_pScene->removeItem(temp);
        delete temp;
    }
}

void ImgArea::clearShapes()
{
    for (auto &temp : m_pScene->items()) {
        if (temp != m_pImageItem) {
            m_pScene->removeItem(temp);
            delete temp;
        }
    }
}

void ImgArea::setRunState(bool isStart)
{
    if (isStart) {
        m_pTipLab->setText("运行监视中");
        m_pTipLab->setStyleSheet("color:#80FF00;font-size:20px;font-weight:10px;");

//        m_pSampleLab->hide();
//        m_pResultLab->hide();

    } else {
        m_pTipLab->setText("保护停止");
        m_pTipLab->setStyleSheet("color:red;font-size:20px;font-weight:10px;");

//        m_pSampleLab->hide();
//        m_pResultLab->hide();
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
}

void ImgArea::setDetectRes(bool isOK)
{
    if (isOK) {
        m_pResultLab->setText("检模OK");
        m_pResultLab->setStyleSheet("color:#80FF00;font-family:Microsoft YaHei;font-size:20px;font-weight:10px;");

    } else {
        m_pResultLab->setText("检模NG");
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
    imgData.sceneId  = SideBar::getInstance()->getIsDetectMold() ? 1 : 2;
    imgData.moldId   = SideBar::getInstance()->getCurMoldNum();
    imgData.imgPath  = QString("%1/%2.png").arg(MyDataBase::imgFilePath).arg(fileName);
    imgData.time     = timeStr;

    MyDataBase::getInstance()->addImgMoldData(imgData);
    m_pCurImage.save(imgData.imgPath);

    return m_pCurImage;
}

QList<ShapeItemData> ImgArea::getShapeItems()
{
    QList<ShapeItemData> shapeList;
    for (QGraphicsItem *temp : m_pScene->items()) {
        if (temp != m_pImageItem) {

            MyGraphicsItem *item = static_cast<MyGraphicsItem *>(temp);
            QPoint center = item->getRealCenter().toPoint();

            ShapeItemData itemData;
            itemData.cameraId = TitleBar::getInstance()->getCurCameraId();
            itemData.sceneId  = SideBar::getInstance()->getIsDetectMold() ? 1 : 2;
            itemData.moldId   = SideBar::getInstance()->getCurrentIdx();
            itemData.type     = item->getType();
            itemData.center   = QString("%1,%2").arg(center.x()).arg(center.y());
            itemData.accuracy = BottomBar::getInstance()->getAccuracy();
            itemData.pixel    = BottomBar::getInstance()->getPixel();

            switch (item->getType()) {
            case MyGraphicsItem::ItemType::Rectangle:{
                qDebug() << "item type: " << item->getType();
                qDebug() << "real center:" << item->getRealCenter().x() << " " << item->getRealCenter().y();
                qDebug() << "center" << item->getCenter().x() << " " << item->getCenter().y();
                qDebug() << "edge" << item->getEdge().x() << " " << item->getEdge().y();

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
            case MyGraphicsItem::ItemType::Polygon: {
                QList<QPointF> myList = item->getMyPointList();

                itemData.edge      = QString("");
                itemData.pointList = pointListToStr(myList);

                MyDataBase::getInstance()->addShapeItemData(itemData);
            }
                break;
            default:
                break;

            }

//            if (item->getType() == MyGraphicsItem::ItemType::Polygon) {
//                QList<QPointF> myList = item->getMyPointList();
//                qDebug() << "point list size" << myList.size();
//            } else if (item->getType() == MyGraphicsItem::ItemType::Curve) {
//                QList<QPointF> myList = item->getMyPointList();
//                qDebug() << "point list size" << myList.size();
//            }
        }
    }
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

    qDebug() << "image path: " << resImgData.imgPath;
    if (img.load(resImgData.imgPath)) {
        QPixmap imgPix = (QPixmap::fromImage(img));
        imgPix = imgPix.scaled(this->size());
        m_pImageItem->setPixmap(imgPix);

        qDebug() << "show image";
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
            m_pScene->addItem(myRect);
        }
            break;
        case MyGraphicsItem::ItemType::Polygon: {
            QStringList myStrList = itemDataList[i].pointList.split(',');
            QPointF myEdgePoint;
            QList<QPointF> myEdgePointList;

            if (myStrList.size() > 2) {
                MyPolygon *myPolygon = new MyPolygon(MyGraphicsItem::ItemType::Polygon);

                for (int i = 0; i < myStrList.size() - 1; i+=2) {
                    if (i + 1 < myStrList.size()) {
                        myEdgePoint = QPointF(myStrList[i].toInt(), myStrList[i+1].toInt());
                        myEdgePointList.append(myEdgePoint);
                        myPolygon->pushPoint(myEdgePoint, myEdgePointList, false);
                    }
                }
                myPolygon->pushPoint(myEdgePoint, myEdgePointList, true);
                m_pScene->addItem(myPolygon);
                connect(m_pScene, &MyGraphicsScene::updatePolyPoint, myPolygon, &MyPolygon::pushPoint);
            }
        }
            break;
        default:
            break;
        }
    }
}

QImage ImgArea::saveAsImage()
{
    qDebug() << m_sceneSize.width() << " " << m_sceneSize.height();
    QImage image(m_sceneSize, QImage::Format_RGB32);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    m_pScene->render(&painter);

    image.save("D:/image/save03.png");

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
    int               iCameraCounts = 4;
    int               iStatus=-1;
    tSdkCameraDevInfo tCameraEnumList[4];

    //sdk初始化  0 English 1中文
    CameraSdkInit(1);

    //枚举设备，并建立设备列表
    CameraEnumerateDevice(tCameraEnumList,&iCameraCounts);

    qDebug() << "CameraCounts: " << iCameraCounts;

    //没有连接设备
    if(iCameraCounts==0){
        return -1;
    }

    //相机初始化。初始化成功后，才能调用任何其他相机相关的操作接口
    iStatus = CameraInit(&tCameraEnumList[0],-1,-1,&g_hCamera);

    //初始化失败
    if(iStatus!=CAMERA_STATUS_SUCCESS){
        return -1;
    }

    char* ipInfo[6];
    for (int i = 0; i < 6; i++) {
        ipInfo[i] = QString("0").toUtf8().data();
    }

    CameraGigeGetIp(&tCameraEnumList[0], ipInfo[0], ipInfo[1], ipInfo[2], ipInfo[3], ipInfo[4], ipInfo[5]);

    for (int i = 0; i < 6; i++) {
        qDebug() << ipInfo[i];
    }

    //获得相机的特性描述结构体。该结构体中包含了相机可设置的各种参数的范围信息。决定了相关函数的参数
    CameraGetCapability(g_hCamera,&g_tCapability);

    g_pRgbBuffer = (unsigned char*)malloc(g_tCapability.sResolutionRange.iHeightMax*g_tCapability.sResolutionRange.iWidthMax*3);
    g_readBuf = (unsigned char*)malloc(g_tCapability.sResolutionRange.iHeightMax*g_tCapability.sResolutionRange.iWidthMax*3);

    /*让SDK进入工作模式，开始接收来自相机发送的图像
    数据。如果当前相机是触发模式，则需要接收到
    触发帧以后才会更新图像。    */
    CameraPlay(g_hCamera);

    /*
        设置图像处理的输出格式，彩色黑白都支持RGB24位
    */
    if(g_tCapability.sIspCapacity.bMonoSensor){
        CameraSetIspOutFormat(g_hCamera,CAMERA_MEDIA_TYPE_MONO8);
    }else{
        CameraSetIspOutFormat(g_hCamera,CAMERA_MEDIA_TYPE_RGB8);
    }
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
    mask = getShapeMask(itemData, QImage());



    return myMOG2Data;
}

int ImgArea::detectImage(QImage imgFg)
{
    int detectRes = DetectRes::NG;

    QImage imgBg;
    imgFg = imgFg.scaled(this->size());

    ShapeItemData itemData;
    itemData.cameraId = TitleBar::getInstance()->getCurCameraId();
    itemData.sceneId  = SideBar::getInstance()->getCurSceneID();
    itemData.moldId   = 1;

    QList<ShapeItemData> itemDataList = MyDataBase::getInstance()->queShapeItemData(itemData);

    ImageMoldData imgData;
    imgData.cameraId = TitleBar::getInstance()->getCurCameraId();
    imgData.sceneId  = SideBar::getInstance()->getCurSceneID();

    QList<ImageMoldData> imgDataList = MyDataBase::getInstance()->queAllImgMoldData(imgData);

    if (imgDataList.size() == 0) {
        return -1;
    }

    QList<Mat> shapeMaskList;
    QVector<QVector<QPointF>> resPointList;

    for (int i = 0; i < itemDataList.size(); i++) {
        Mat shapeMask = getShapeMask(itemDataList[i], imgFg);
        shapeMaskList.append(shapeMask);
    }

    for (int i = 0; i < imgDataList.size(); i++) {

        imgBg = QImage(imgDataList[i].imgPath).scaled(this->size());

        int imgMoldDeteRes = DetectRes::OK;

        for (int j = 0; j < itemDataList.size(); j++) {
            Mat mask, srcFg, dstBg, dstFg;
            mask = shapeMaskList[j];

            srcFg = qimToMat(imgFg);

            qimToMat(imgBg).copyTo(dstBg, mask);
            qimToMat(imgFg).copyTo(dstFg, mask);

            double acc  = itemDataList[j].accuracy;
            int minArea = itemDataList[j].pixel;

            m_pMOG2 = createBackgroundSubtractorMOG2(50, acc, false);

            Mat frameBg = dstBg.clone();
            Mat frameFg = dstFg.clone();

            for (int k = 0; k < m_pMOG2->getHistory(); k++) {
                m_frame = frameBg.clone();
                m_pMOG2->apply(m_frame, m_fgMaskMOG2);
            }

            m_frame = frameFg.clone();
            m_pMOG2->apply(m_frame, m_fgMaskMOG2, 0);

    //        rectangle(m_frame, cv::Point(10, 2), cv::Point(100, 20),
    //                  cv::Scalar(255, 255, 255), -1);

            // get result
            vector<vector<Point>> contours;
            vector<Vec4i> hierarchy;
            findContours(m_fgMaskMOG2, contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());
            Mat imageContours = Mat::zeros(m_fgMaskMOG2.size(), CV_8UC1);
            Mat contoursMat = Mat::zeros(m_fgMaskMOG2.size(), CV_8UC1);
            vector<vector<Point>> approxPoint(contours.size());


            for (int k = 0; k < int(contours.size()); k++) {
                if (contourArea(contours[k]) > minArea) {

                    qDebug() << "contourArea(contours[k]):" << contourArea(contours[k]);

                    // 绘制轮廓
                    drawContours(srcFg, contours, k, Scalar(0, 0, 255), 2, 8, hierarchy);

                    // 出现NG
                    imgMoldDeteRes = DetectRes::NG;

                    if (i == imgDataList.size() - 1) {
                        QVector<QPointF> singlePointList;
                        for (int m = 0; m < int(contours[k].size()); m++) {
                            singlePointList.append(QPointF(contours[k][m].x, contours[k][m].y));
                        }
                        resPointList.append(singlePointList);
                    }
                }
            }

            imshow(QString("mask_%1_%2").arg(i).arg(j).toStdString(), mask);
            imshow(QString("fgMaskMOG2_%1_%2").arg(i).arg(j).toStdString(), m_fgMaskMOG2);
            imshow(QString("detect result_%1_%2").arg(i).arg(j).toStdString(), srcFg);

            m_pMOG2->clear();
        }

        if (imgMoldDeteRes == DetectRes::OK) {
            detectRes = DetectRes::OK;
            break;
        }
    }

    if (detectRes == DetectRes::NG) {
        drawDetectResult(resPointList);
    }

    qDebug() << "detect res: " << detectRes;
    return detectRes;
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

Mat ImgArea::getShapeMask(ShapeItemData itemData, QImage img)
{
    cv::Mat mask, dst;
//    QImage img = m_pImageItem->pixmap().toImage();

    img = img.scaled(this->size());

    qimToMat(img).copyTo(mask);
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
    default:
        break;
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

QImage ImgArea::getCurImage()
{
    return m_pCurImage;
}

int ImgArea::getCameraStatus()
{
    return status;
}

void ImgArea::drawDetectResult(QVector<QVector<QPointF>> resPointList)
{
    QPen polyPen = QPen(Qt::red, 2);
    qDebug() << "resPointList.size(): " << resPointList.size();
    for (int i = 0; i < resPointList.size(); i++) {
        QPolygonF resPoly = QPolygonF(resPointList[i]);
        m_detectResItemList.append(m_pScene->addPolygon(resPoly, polyPen));
    }
}

void ImgArea::clearDetectResult()
{
    for (int i = 0; i < m_detectResItemList.size(); i++) {
        m_pScene->removeItem(m_detectResItemList[i]);
    }

    m_detectResItemList.clear();
    m_pResultLab->hide();
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

