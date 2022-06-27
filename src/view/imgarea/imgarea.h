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

#include "src/view/imgarea/mygraphicsitem.h"
#include "src/view/imgarea/mygraphicsscene.h"
#include "src/camera/capturethread.h"
#include "src/camera/Include/CameraApi.h"
#include "src/database/mydatabase.h"

typedef struct _WIDTH_HEIGHT{
    int     display_width;
    int     display_height;
    int     xOffsetFOV;
    int     yOffsetFOV;
    int     sensor_width;
    int     sensor_height;
    int     buffer_size;
}Width_Height;

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

    void eraseShape();
    void clearShapes();

    // 设置运行状态
    void setRunState(bool isStart);

    // 设置场景尺寸
    void setSceneSize();

    // 设置检模结果
    void setDetectRes(bool isOK);

    // 设置样本显示
    void setSampleLab(bool isDetectMold, int curIdx = 1);

    // 获得图像图形模板
    QGraphicsPixmapItem *  getImageItem();
    QList<ShapeItemData> getShapeItems();

    // 加载图像图形模板
    void loadImageItem(QGraphicsPixmapItem *  imageItem);
    void loadShapeItem(QList<QGraphicsItem *> shapeItemList);

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

public:
    int status;

private:
    // SDK初始化
    int initSDK();
    void cameraStatues();
    int initParameter(int hCamera,tSdkCameraCapbility * pCameraInfo);

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

    QSize m_sceneSize;

    QTimer *m_timer;
    CaptureThread *m_thread;

    // 曝光时间
    double m_pfExposureTime;
    double m_exposureTimeMin;
    double m_exposureTimeMax;
    double m_expLineTime;

    // 增益值
    int m_pusAnalogGain;
    int m_analogGainMin;
    int m_analogGainMax;

};




#endif // IMG_AREA
