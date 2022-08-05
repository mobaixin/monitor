#ifndef IMG_AREA
#define IMG_AREA

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QLineEdit>
#include <QProgressBar>
#include <QImage>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QGraphicsPathItem>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QList>
#include <QTimer>
#include <QThread>
#include <windows.h>
#include <opencv2\opencv.hpp>
#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "src/view/imgarea/mygraphicsitem.h"
#include "src/view/imgarea/mygraphicsscene.h"
#include "src/camera/capturethread.h"
#include "src/camera/Include/CameraApi.h"
#include "src/database/mydatabase.h"

// netsh interface ipv4 set address name="以太网 2" source=static address=192.168.0.106 mask=255.255.255.0 gateway=192.168.0.1

using namespace cv;
using namespace std;

// 相机SDK数据
typedef struct _WIDTH_HEIGHT{
    int     display_width;
    int     display_height;
    int     xOffsetFOV;
    int     yOffsetFOV;
    int     sensor_width;
    int     sensor_height;
    int     buffer_size;
}Width_Height;

// MOG2数据
typedef struct MyMOG2Data {
    int cameraId;
    int sceneId;
    Ptr<BackgroundSubtractorMOG2> myMOG2;
    Mat fgMaskMat;
    double accuracy;
    int pixel;
} MyMOG2Data;

// 相机显示数据
typedef struct CameraViewData {
    int cameraId;
    int camState;
    QGraphicsView *camView;
    MyGraphicsScene *camScene;
    QGraphicsPixmapItem *imageItem;
    QList<QGraphicsItem *> allShapeItemList;
    CaptureThread *camThread;
    QImage curImage;
    QImage curDetectImage;
    int curDetectSceneId;
    double scaleValue;
    bool isShowCamImage;
} CameraViewData;

// 相机检测数据
typedef struct CameraDetectData {
    int cameraId;
    int cameraRunState;
    QList<Mat> moldShapeMaskList;
    QList<Mat> prodShapeMaskList;
    QList<MyMOG2Data> moldMOG2DataList;
    QList<MyMOG2Data> prodMOG2DataList;
} CameraDetectData;

// 相机运行状态
enum CameraState {
    OffLine = 0,
    Running = 1,
    Pause   = 2,
};

// 检测结果
enum DetectRes {
    NG = 0,
    OK = 1,
};

class DetectImageWork;

class ImgArea : public QWidget
{
    Q_OBJECT
public:
    static ImgArea *getInstance(QWidget *parent = nullptr);

    explicit ImgArea(QWidget *parent = nullptr);
    virtual ~ImgArea();

    // 初始化组件
    void setWidgetUi();

    // 设置组件样式
    void setWidgetStyle();

    // 设置初始数据
    void setData();

    // 获取scene
    MyGraphicsScene *getScene();

    // 设置背景图片
    void loadImage(QString filePath, int cameraId = -1);

    // 擦除单个图形item
    void eraseShape(int cameraId = -1);

    // 清除所有图形item
    void clearShapes(int cameraId = -1);

    // 设置运行状态
    void setRunState(int state, int cameraId = -1);

    // 设置监视状态
    void setMonitorState(bool isMonitor, int cameraId = 1);

    // 设置检测状态
    void setDetectState(bool isDetect);

    // 设置场景尺寸
    void setSceneSize();

    // 设置检模结果
    void setDetectRes(bool isOK, int sceneId = -1);

    // 设置样本显示
    void setSampleLab(bool isDetectMold, int curIdx = 1);

    // 获得图像图形模板
    QImage getImageItem(int cameraId = -1);
    QList<ShapeItemData> getShapeItems(int cameraId = -1);

    // 加载图像图形模板
    void loadImageItem(ImageMoldData imgData, int cameraId = -1);
    void loadShapeItem(ShapeItemData itemData, int cameraId = -1);

    // 保存为图片
    QImage saveAsImage(QString imgPath);

    // 设置分辨率
    void setResolution(int index);

    // 设置曝光时间
    void setExposeTime(int value, int cameraIdx = 1);

    // 设置相机增益
    void setCameraGain(int gain, int cameraIdx = 1);

    // 获取曝光数据
    QList<double> getExposureTime();

    // 获取增益数据
    QList<int> getCameraGain();

    // 保存相机参数
    void saveCameraPara(int cameraIdx = 1);

    // 开始运行
    void startCamera(int cameraId = 1);

    // 停止运行
    void pauseCamera(int cameraId = 1);

    // 更新图形图片模板
    void updateShapeImgMold(int cameraId, int sceneId);

    // 检测当前的图片 手动检测时sceneId为-1 检测次数默认为1
    int detectCurImage(int cameraId, int sceneId = -1, int detectTimes = 1);

    // 自动检测当前图片
    int autoDetectImage(int cameraId, int sceneId, double delayTime, int reDetectTimes);

    // 图片检测
    int detectImage(QImage imgFg, int cameraId, int sceneId);

    // 获取检测结果
    void getDetectResult(int result, QVector<QVector<QPointF>> resPointList, QList<int> resAreaSizeList);

    // 获取当前检测图片
    QImage getCurDetectImage(int cameraId);

    // 获取当前检测相机id
    int getCurDetectCameraId();

    // 获取当前检测场景id
    int getCurDetectSceneId();

    // 获取item数
    int getShapeItemNum(int cameraId);

    // 获取opencv mask
    Mat getShapeMask(ShapeItemData itemData, QImage img, QList<ShapeItemData> maskItemData);

    // 设置显示状态
    void setShowState(bool isShow, int cameraId = -1);

    // 获取显示状态
    bool getShowState(int cameraId = -1);

    // 获取当前图片
    QImage getCurImage(int cameraId);

    // 获取相机数
    int getCameraCounts();

    // 获取相机状态
    int getCameraState(int cameraId);

    // 描绘检测结果
    void drawDetectResult(QVector<QVector<QPointF>> resPointList);

    // 清除检测结果
    void clearDetectResult();

    // 设置图形不允许移动
    void setShapeNoMove(bool noMove);

    // 获取当前检测场景id
    int getDetectSceneId();

    // 设置当前检测场景及延时
    void setSceneDelayTime(int sceneId, double delayTime);

    // 添加图形item到list
    void addShapeItemToList(QGraphicsItem *newItem, int cameraId = -1);

    // 获取选中的图形item
    QList<QGraphicsItem *> getSelectItemList();

    // 完成绘制多边形时判断状态
    bool judgePolygonState(MyGraphicsItem *newPolygon);

    // 复制当前选中的图形
    void copySelectedShapeItem();

    // 显示全部相机画面
    void showAllCameraView();

    // 显示单个相机画面
    void showSingleCameraView(int cameraId);

public:
    int status;

signals:
    void setCameraCountsSig(int cameraCounts);
    void setSceneRectSizeSig(QSize size);
    void startUpdateMoldSig(int cameraId, int sceneId, QList<ShapeItemData> itemDataList, QList<ImageMoldData> imgDataList);
    void startDetectImageSig(QImage imgFg, int cameraId, int sceneId, int &detectRes);

private:
    // 本机IP地址初始化
    int initLocalNetwork();

    // SDK初始化
    int initSDK();
    void cameraStatues();
    int initParameter(int hCamera,tSdkCameraCapbility * pCameraInfo);

    QString pointListToStr(QList<QPointF> pointList);

    // 获取MOG2
    Ptr<BackgroundSubtractorMOG2> getMOG2Data(ShapeItemData itemData);

    static QImage matToQim(Mat & mat);
    static Mat qimToMat(QImage & qim);

    // 设置倒计时显示
    void setSigDelayTimeLab();

private slots:
    void imageProcess(QImage img, int cameraId);
    void imageProcess1(QImage img, int cameraId);
    void imageProcess2(QImage img, int cameraId);
    void imageProcess3(QImage img, int cameraId);
    void imageProcess4(QImage img, int cameraId);

private:
    // 相机状态
    QLabel *m_pTipLab;
    QList<QLabel *> m_cameraStateLabList;
    // 信号倒计时
    QLabel *m_pSigTimeLab;
    // 图片模板序号
    QLabel *m_pSampleLab;
    // 检测结果
    QLabel *m_pResultLab;

    QImage *m_pMainImg;

    QGridLayout *m_pImgAreaLayout;

    QGraphicsView *m_pView;
    MyGraphicsScene *m_pScene;
    QGraphicsPixmapItem *m_pImageItem;
    QImage m_pCurImage;

    QList<QGraphicsView *> m_viewList;
    QList<MyGraphicsScene *> m_sceneList;
    QList<QGraphicsPixmapItem *> m_imageItemList;
    QList<QImage> m_curImageList;

    // 单个scene的尺寸
    QSize m_sceneSize;

    // 当前检测相机id
    int m_detectCameraId;

    // 当前检测场景id
    int m_detectSceneId;

    // 当前检测图片
    QImage m_curDetectImage;

    // 相机数
    int m_cameraCounts;

    // 相机监视状态
    QList<int> m_cameraStateList;

    QTimer *m_resTimer;
    CaptureThread *m_thread;
    QList<CaptureThread *>m_cameraThreadList;

    // 信号倒计时
    QTimer *m_sigDelayTimer;
    double m_delayTime;
    int m_reDetectTimes;

    // 本机IP地址
//    QString m_ifaceIp  = QString("192.168.0.10%1");
    QString m_ifaceIp  = QString("192.168.%1.10%2");

    // 相机IP地址
//    QString m_cameraIp = QString("192.168.0.11%1");
    QString m_cameraIp   = QString("192.168.%1.21%2");
    QString m_cameraMask = QString("255.255.255.0");

    // 网关地址
    QString m_gateway  = QString("192.168.%1.1");

    // 检测图片线程
    QThread *m_detectThread;
    DetectImageWork *m_detectImageWork;

    // NG区域的边缘点
    QVector<QVector<QPointF>> m_resPointList;

    // NG区域的范围大小
    QList<int> m_resAreaSizeList;

    bool m_isShowImage;

    // 曝光时间
    double m_pfExposureTime;
    double m_exposureTimeMin;
    double m_exposureTimeMax;
    double m_expLineTime;

    // 增益值
    int m_pusAnalogGain;
    int m_analogGainMin;
    int m_analogGainMax;

    // opencv
    int m_minArea;
    Mat m_frame;
    Mat m_fgMaskMOG2;
    Mat m_maskCountour;
//    Ptr<BackgroundSubtractorMOG2> m_pMOG2;

    QList<Mat> m_moldShapeMaskList;
    QList<Mat> m_prodShapeMaskList;
    QList<MyMOG2Data> m_moldMOG2DataList;
    QList<MyMOG2Data> m_prodMOG2DataList;

    QList<CameraDetectData> m_cameraDetectDataList;

    QList<QGraphicsPolygonItem *> m_detectResItemList;
    QList<QGraphicsTextItem *> m_detectResTxtItemList;

    // 存放所有的图形item
    QList<QGraphicsItem *> m_allShapeItemList;

    // 所有相机显示相关的信息
    QList<CameraViewData> m_cameraViewDataList;
};


class DetectImageWork : public QObject
{
    Q_OBJECT
public:
    explicit DetectImageWork(QObject *parent = nullptr);
    virtual ~DetectImageWork();

    // 设置相机数
    void setCameraCounts(int cameraCounts);

    // 设置检测场景尺寸
    void setSceneRectSize(QSize size);

    // 获取opencv mask
    Mat getShapeMask(ShapeItemData itemData, QImage img, QList<ShapeItemData> maskItemDataList);

    // 更新图形图片模板
    void updateShapeImgMold(int cameraId, int sceneId, QList<ShapeItemData> itemDataList, QList<ImageMoldData> imgDataList);

    // 图片检测
    void detectImage(QImage imgFg, int cameraId, int sceneId, int &detectRes);

private:
    // Mat列表内存回收
    void releaseMatList(QList<Mat> &matList);

signals:
    void resultReadySig(int result, QVector<QVector<QPointF>> resPointList, QList<int> resAreaSizeList);

private:
    static QImage matToQim(Mat & mat);
    static Mat qimToMat(QImage & qim);

private:
    QSize m_sceneRectSize;

    // opencv
    int m_minArea;
    Mat m_frame;
    Mat m_fgMaskMOG2;
    Mat m_maskCountour;

    QList<Mat> m_moldShapeMaskList;
    QList<Mat> m_prodShapeMaskList;
    QList<MyMOG2Data> m_moldMOG2DataList;
    QList<MyMOG2Data> m_prodMOG2DataList;

    QList<CameraDetectData> m_cameraDetectDataList;
};


static std::string base64Encode(const unsigned char* Data, int DataByte) {
    //编码表
    const char EncodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    //返回值
    std::string strEncode;
    unsigned char Tmp[4] = { 0 };
    int LineLength = 0;
    for (int i = 0; i < (int)(DataByte / 3); i++) {
        Tmp[1] = *Data++;
        Tmp[2] = *Data++;
        Tmp[3] = *Data++;
        strEncode += EncodeTable[Tmp[1] >> 2];
        strEncode += EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
        strEncode += EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
        strEncode += EncodeTable[Tmp[3] & 0x3F];
        if (LineLength += 4, LineLength == 76) { strEncode += "\r\n"; LineLength = 0; }
    }
    //对剩余数据进行编码
    int Mod = DataByte % 3;
    if (Mod == 1) {
        Tmp[1] = *Data++;
        strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
        strEncode += EncodeTable[((Tmp[1] & 0x03) << 4)];
        strEncode += "==";
    }
    else if (Mod == 2) {
        Tmp[1] = *Data++;
        Tmp[2] = *Data++;
        strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
        strEncode += EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
        strEncode += EncodeTable[((Tmp[2] & 0x0F) << 2)];
        strEncode += "=";
    }


    return strEncode;
}

//imgType 包括png bmp jpg jpeg等opencv能够进行编码解码的文件
static std::string Mat2Base64(const cv::Mat &img, std::string imgType) {
    //Mat转base64
    std::string img_data;
    std::vector<uchar> vecImg;
    std::vector<int> vecCompression_params;
    vecCompression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    vecCompression_params.push_back(90);
    imgType = "." + imgType;
    cv::imencode(imgType, img, vecImg, vecCompression_params);
    img_data = base64Encode(vecImg.data(), vecImg.size());
    return img_data;
}


//static cv::Mat Base2Mat(std::string &base64_data) {
//    cv::Mat img;
//    std::string s_mat;
//    s_mat = base64Decode(base64_data.data(), base64_data.size());
//    std::vector<char> base64_img(s_mat.begin(), s_mat.end());
//    img = cv::imdecode(base64_img, CV_LOAD_IMAGE_COLOR);
//    return img;
//}

#endif // IMG_AREA
