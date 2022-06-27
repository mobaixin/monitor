#include <QMessageBox>
#include <QPixmap>
#include <QDebug>
#include <QApplication>
#include <QList>

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

//SDK
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
//    m_pScene->setBackgroundBrush(Qt::black);

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
    if(initSDK()==-1){
        status =0;
    } else {
        initParameter(g_hCamera,&g_tCapability);
//        CameraSetOnceWB(g_hCamera);

        m_thread->start();
        m_thread->stream();
        status =1;
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
        m_pScene->removeItem(m_pScene->selectedItems().first());
    }
}

void ImgArea::clearShapes()
{
    for (auto &temp : m_pScene->items()) {
        if (temp != m_pImageItem) {
            m_pScene->removeItem(temp);
        }
    }
}

void ImgArea::setRunState(bool isStart)
{
    if (isStart) {
        m_pTipLab->setText("运行监视中");
        m_pTipLab->setStyleSheet("color:#80FF00;font-size:20px;font-weight:10px;");

        m_pSampleLab->hide();
        m_pResultLab->hide();

    } else {
        m_pTipLab->setText("保护停止");
        m_pTipLab->setStyleSheet("color:red;font-size:20px;font-weight:10px;");

        m_pSampleLab->show();
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
}

QGraphicsPixmapItem *ImgArea::getImageItem()
{
    return m_pImageItem;
}

QList<ShapeItemData> ImgArea::getShapeItems()
{
    QList<ShapeItemData> shapeList;
    for (QGraphicsItem *temp : m_pScene->items()) {
        if (temp != m_pImageItem) {

            MyGraphicsItem *item = static_cast<MyGraphicsItem *>(temp);
            switch (item->getType()) {
            case MyGraphicsItem::ItemType::Rectangle:{
                qDebug() << "item type: " << item->getType();
                qDebug() << "real center:" << item->getRealCenter().x() << " " << item->getRealCenter().y();
                qDebug() << "center" << item->getCenter().x() << " " << item->getCenter().y();
                qDebug() << "edge" << item->getEdge().x() << " " << item->getEdge().y();

                QPointF center = item->getRealCenter();
                QPointF edge = QPointF(item->getRealCenter().x() + item->getEdge().x() - item->getCenter().x(),
                                       item->getRealCenter().y() + item->getEdge().y() - item->getCenter().y());

                ShapeItemData itemData;
                itemData.isDetect = 2;
                if (SideBar::getInstance()->getIsDetectMold()) {
                    itemData.isDetect = 1;
                }

                itemData.moldId = SideBar::getInstance()->getCurrentIdx();
                itemData.type   = item->getType();
                itemData.center = QString("%1,%2").arg(center.x()).arg(center.y());
                itemData.edge   = QString("%1,%2").arg(edge.x()).arg(edge.y());
                itemData.pointList = "";
                itemData.accuracy  = BottomBar::getInstance()->getAccuracy();
                itemData.pixel     = BottomBar::getInstance()->getPixel();

                qDebug() << MyDataBase::getInstance()->addShapeItemData(itemData);

            }
                break;
            default:
                break;

            }

            if (item->getType() == MyGraphicsItem::ItemType::Polygon) {
                QList<QPointF> myList = item->getMyPointList();
                qDebug() << "point list size" << myList.size();
            } else if (item->getType() == MyGraphicsItem::ItemType::Curve) {
                QList<QPointF> myList = item->getMyPointList();
                qDebug() << "point list size" << myList.size();
            }
        }
    }
    return shapeList;
}

void ImgArea::loadImageItem(QGraphicsPixmapItem *imageItem)
{
    if (imageItem != nullptr) {
        m_pScene->clear();
        m_pScene->addItem(imageItem);
    }
}

void ImgArea::loadShapeItem(QList<QGraphicsItem *> shapeItemList)
{
    if (shapeItemList.size() != 0) {
        for (QGraphicsItem *item : shapeItemList) {
            m_pScene->addItem(item);
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

    image.save("D:/image/save02.png");

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



    QPixmap imgPix = (QPixmap::fromImage(img));
    imgPix = imgPix.scaled(this->size());
    m_pImageItem->setPixmap(imgPix);

    g_disply_fps++;
}

