#ifndef IMG_AREA
#define IMG_AREA

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
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
#include <windows.h>
#include <opencv2\opencv.hpp>

#include "src/view/imgarea/mygraphicsitem.h"
#include "src/view/imgarea/mygraphicsscene.h"
#include "src/camera/capturethread.h"
#include "src/camera/Include/CameraApi.h"
#include "src/database/mydatabase.h"

using namespace cv;
using namespace std;

typedef struct _WIDTH_HEIGHT{
    int     display_width;
    int     display_height;
    int     xOffsetFOV;
    int     yOffsetFOV;
    int     sensor_width;
    int     sensor_height;
    int     buffer_size;
}Width_Height;

typedef struct myMOG2Data {
    Ptr<BackgroundSubtractorMOG2> myMOG2;
    int accuracy;
    int pixel;
} myMOG2Data;

enum DetectRes {
    NG = 0,
    OK = 1,
};

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
    void loadImage(QString filePath);

    // 擦除单个图形item
    void eraseShape();

    // 清除所有图形item
    void clearShapes();

    // 设置运行状态
    void setRunState(bool isStart);

    // 设置监视状态
    void setMonitorState(bool isMonitor);

    // 设置检测状态
    void setDetectState(bool isDetect);

    // 设置场景尺寸
    void setSceneSize();

    // 设置检模结果
    void setDetectRes(bool isOK);

    // 设置样本显示
    void setSampleLab(bool isDetectMold, int curIdx = 1);

    // 获得图像图形模板
    QImage getImageItem();
    QList<ShapeItemData> getShapeItems();

    // 加载图像图形模板
    void loadImageItem(ImageMoldData imgData);
    void loadShapeItem(ShapeItemData itemData);

    // 保存为图片
    QImage saveAsImage();

    // 设置分辨率
    void setResolution(int index);

    // 设置曝光时间
    void setExposeTime(int value, int cameraIdx = 0);

    // 设置相机增益
    void setCameraGain(int gain, int cameraIdx = 0);

    // 获取曝光数据
    QList<double> getExposureTime();

    // 获取增益数据
    QList<int> getCameraGain();

    // 保存相机参数
    void saveCameraPara();

    // 开始运行
    void startCamera();

    // 停止运行
    void pauseCamera();

    // 图片检测
    int detectImage(QImage imgFg);

    // 获取item数
    int getShapeItemNum();

    // 获取opencv mask
    Mat getShapeMask(ShapeItemData itemData, QImage img);

    // 设置显示状态
    void setShowState(bool isShow);

    // 获取当前图片
    QImage getCurImage();

    // 获取相机状态
    int getCameraStatus();

    // 描绘检测结果
    void drawDetectResult(QVector<QVector<QPointF>> resPointList);

    // 清除检测结果
    void clearDetectResult();

public:
    int status;

private:
    // SDK初始化
    int initSDK();
    void cameraStatues();
    int initParameter(int hCamera,tSdkCameraCapbility * pCameraInfo);

    QString pointListToStr(QList<QPointF> pointList);

    // 获取MOG2
    Ptr<BackgroundSubtractorMOG2> getMOG2Data(ShapeItemData itemData);

    static QImage matToQim(Mat & mat);
    static Mat qimToMat(QImage & qim);

private slots:
    void imageProcess(QImage img);

private:
    QLabel *m_pTipLab;
    QLabel *m_pSampleLab;
    QLabel *m_pResultLab;

    QImage *m_pMainImg;

    QHBoxLayout *m_pImgAreaLayout;

    QGraphicsView *m_pView;
    MyGraphicsScene *m_pScene;
    QGraphicsPixmapItem *m_pImageItem;
    QImage m_pCurImage;

    QSize m_sceneSize;

    QTimer *m_timer;
    CaptureThread *m_thread;

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
    Ptr<BackgroundSubtractorMOG2> m_pMOG2;

    QList<myMOG2Data> myMOG2DataList;

    QList<QGraphicsPolygonItem *> m_detectResItemList;
};




#endif // IMG_AREA
