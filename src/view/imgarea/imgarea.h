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

#include <opencv2\opencv.hpp>
//#include <iostream>
//#include <stdlib.h>
//#include <string.h>

#include "src/view/imgarea/mygraphicsitem.h"
#include "src/view/imgarea/mygraphicsscene.h"
#include "src/camera/capturethread.h"
#include <windows.h>
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
    int cameraId;       // 相机ID
    int sceneId;        // 场景ID
    Ptr<BackgroundSubtractorMOG2> myMOG2;   // 学习数据
    Mat fgMaskMat;      // 图形掩膜
    double accuracy;    // 精确度
    int pixel;          // 像素值
} MyMOG2Data;

// 相机参数数据
typedef struct CameraParaData {
    // 曝光时间
    double pfExposureTime;
    double exposureTimeMin;
    double exposureTimeMax;
    double expLineTime;

    // 增益值
    int pusAnalogGain;
    int analogGainMin;
    int analogGainMax;
} CameraParaData;

// 相机显示数据
typedef struct CameraViewData {
    int cameraId;                   // 相机id
    int camState;                   // 相机状态
    QGraphicsView *camView;         // 视频流显示
    MyGraphicsScene *camScene;      // 场景
    QGraphicsPixmapItem *imageItem; // 显示视频流的item
    QList<QGraphicsItem *> allShapeItemList;    // 所有的图形item
    CaptureThread *camThread;       // 视频流线程
    QImage curImage;                // 当前获取的图片
    QImage curDetectImage;          // 当前检测的图片
    int curDetectSceneId;           // 当前检测场景
    double scaleValue;              // 缩放的值
    bool isShowCamImage;            // 是否显示视频流
    bool isNGState;                 // 是否处于NG状态
    QVector<QVector<QPointF>> resPointList;   // NG区域的边缘点
    QList<int> resAreaSizeList;     // NG区域的范围大小
    QList<QGraphicsPolygonItem *> detectResItemList;    // 检测结果图形
    QList<QGraphicsTextItem *> detectResTxtItemList;    // 检测结果范围
    CameraParaData cameraParaData;  // 相机参数
} CameraViewData;

// 相机检测数据
typedef struct CameraDetectData {
    int cameraId;                       // 相机ID
    int cameraRunState;                 // 相机运行状态
    QList<Mat> moldShapeMaskList;       // 检模图形掩膜列表
    QList<Mat> prodShapeMaskList;       // 产品图形掩膜列表
    QList<MyMOG2Data> moldMOG2DataList; // 检模学习数据列表
    QList<MyMOG2Data> prodMOG2DataList; // 产品学习数据列表
    QList<QPointF> moldShapeCenterList; // 检模图形中心点列表
    QList<QPointF> moldShapeBoundList;  // 检模图形外接矩形
    QList<QPointF> prodShapeCenterList; // 产品图形中心点列表
    QList<QPointF> prodShapeBoundList;  // 产品图形外接矩形
} CameraDetectData;

// 相机运行状态
enum CameraState {
    OffLine = 0,    // 相机断线
    Running = 1,    // 监视运行
    Pause   = 2,    // 保护停止
};

// 检测结果
enum DetectRes {
    NG = 0,         // NG
    OK = 1,         // OK
};

// 检测图片类
class DetectImageWork;

class ImgArea : public QWidget
{
    Q_OBJECT
public:
    // 单例
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
    void setResolution(int cameraId);

    // 设置曝光时间
    void setExposeTime(int value, int cameraId = -1);

    // 设置相机增益
    void setCameraGain(int gain, int cameraId = -1);

    // 获取曝光数据
    QList<double> getExposureTime(int cameraId = -1);

    // 获取增益数据
    QList<int> getCameraGain(int cameraId = -1);

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

    // 获取单个相机的NG状态
    bool getCamNGState(int cameraId);

    //设置单个相机的NG状态
    void setCamNGState(int cameraId, bool state);

public:
    int status;

signals:
    // 设置相机个数信号
    void setCameraCountsSig(int cameraCounts);

    // 设置场景尺寸信号
    void setSceneRectSizeSig(QSize size);

    // 更新图形图像模板信号
    void startUpdateMoldSig(int cameraId, int sceneId, QList<ShapeItemData> itemDataList, QList<ImageMoldData> imgDataList);

    // 开始检测图片信号
    void startDetectImageSig(QImage imgFg, int cameraId, int sceneId, int &detectRes);

private:
    // 本机IP地址初始化
    int initLocalNetwork();

    // SDK初始化
    int initSDK();
    void cameraStatues();

    // 初始化相机参数
    int initParameter(int hCamera,tSdkCameraCapbility * pCameraInfo);

    // 坐标列表转字符串
    QString pointListToStr(QList<QPointF> pointList);

    // QImage和Mat格式转换
    static QImage matToQim(Mat & mat);
    static Mat qimToMat(QImage & qim);

    // 设置倒计时显示
    void setSigDelayTimeLab();

private slots:
    // 视频流显示
    void imageProcess(QImage img, int cameraId);
    void imageProcess1(QImage img, int cameraId);
    void imageProcess2(QImage img, int cameraId);
    void imageProcess3(QImage img, int cameraId);
    void imageProcess4(QImage img, int cameraId);

private:
    // 相机状态
    QList<QLabel *> m_cameraStateLabList;

    // 信号倒计时
    QLabel *m_pSigTimeLab;

    // 图片模板序号
    QLabel *m_pSampleLab;

    // 检测结果
    QList<QLabel *> m_resultLabList;

    QGridLayout *m_pImgAreaLayout;

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

    // 检测结果显示倒计时
    QTimer *m_resTimer;

    // 检测信号倒计时
    QTimer *m_sigDelayTimer;

    // 检测延时
    double m_delayTime;

    // 重检次数
    int m_reDetectTimes;

    // 本机IP地址
//    QString m_ifaceIp  = QString("192.168.0.10%1");
    QString m_ifaceIp  = QString("192.168.%1.20%2");

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

    // 曝光时间
    double m_pfExposureTime;
    double m_exposureTimeMin;
    double m_exposureTimeMax;
    double m_expLineTime;

    // 增益值
    int m_pusAnalogGain;
    int m_analogGainMin;
    int m_analogGainMax;

    // 存放所有的图形item
    QList<QGraphicsItem *> m_allShapeItemList;

    // 所有相机显示相关的信息
    QList<CameraViewData> m_cameraViewDataList;
};

// 检测图片类
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

signals:
    // 检测结果信号
    void resultReadySig(int result, QVector<QVector<QPointF>> resPointList, QList<int> resAreaSizeList);

private:
    // QImage和Mat格式转换
    static QImage matToQim(Mat & mat);
    static Mat qimToMat(QImage & qim);

private:
    QSize m_sceneRectSize;  // 场景尺寸 即图片要缩放的尺寸

    QList<Mat> m_moldShapeMaskList;         // 检模图形掩膜列表
    QList<Mat> m_prodShapeMaskList;         // 产品图形掩膜列表
    QList<MyMOG2Data> m_moldMOG2DataList;   // 检模学习数据列表
    QList<MyMOG2Data> m_prodMOG2DataList;   // 产品学习数据列表

    QList<CameraDetectData> m_cameraDetectDataList; // 相机检测数据列表
};

#endif // IMG_AREA
