#ifndef BOTTOM_BAR
#define BOTTOM_BAR

#include <QWidget>
#include <QGridLayout>

#include "src/view/imgarea/mygraphicsitem.h"
#include "src/view/imgarea/mygraphicsscene.h"
#include "src/view/common/myslider.h"

class BottomBar : public QWidget
{
    Q_OBJECT
public:
    static BottomBar *getInstance(QWidget *parent = nullptr);

    explicit BottomBar(QWidget *parent = nullptr);
    virtual ~BottomBar();

    // 初始化组件
    void setWidgetUi();

    // 设置组件样式
    void setWidgetStyle();

    // 设置初始数据
    void setData();

    // 清除按钮绑定
    void setClearBtnConnect();

    void setBtnEnabled(bool enable);

    // 设置精确度值
    void setAccuracy(int acc);

    // 获得精确度值
    int getAccuracy();

    // 设置像素值
    void setPixel(int pix);

    // 获得像素值
    int getPixel();

    // 创建矩形
    void createRect(QPointF point);

    // 获取创建的矩形
    MyRectangle *getNewMyRect();

    // 创建圆形
    void createCircle(QPointF point);

    // 获取创建的圆形
    MyCircle *getNewMyCircle();

    // 创建圆环
    void createConCircle(QPointF point);

    // 获取创建的圆环
    MyConcentricCircle *getNewMyConCircle();

    // 更新曲线图形状态
    void updateCreateCurve();

    // 更新多边形图形状态
    void updateCreatePolygon();

private:
    // 按钮响应事件
//    void accAddBtnClick();
//    void accSubBtnClick();
//    void pixAddBtnClick();
//    void pixSubBtnClick();

//    void setAccLineValue(int value);
//    void setPixLineValue(int value);

//    void setAccSliderValue();
//    void setPixSliderValue();

    void circleBtnClick();      // 圆形
    void conCircleBtnClick();   // 环形
    void rectBtnClick();        // 矩形
    void polygonBtnClick();     // 多边形
    void curveBtnClick();       // 曲线
    void maskBtnClick();        // 屏蔽区
    void copyBtnClick();        // 复制
    void positionBtnClick();    // 位置

    void updateItemAcc(int acc);    // 更新图形精确度
    void updateItemPix(int pix);    // 更新图形像素值

private:
//    MyGraphicsScene *m_pAreaScene;

    MyRectangle *m_newMyRect;   // 新创建的矩形
    MyCircle *m_newMyCircle;    // 新创建的圆形
    MyConcentricCircle *m_newMyConCircle;   // 新创建的圆环
    MyPolygon *m_newPolygon;    // 新创建的多边形

    bool m_isCreatePolygon;     // 判断创建多边形状态
    bool m_isCreateCurve;       // 判断创建曲线状态
    bool m_isCreateMask;        // 判断创建屏蔽区状态

    bool m_isUpdateAcc;         // 是否更新精确度
    bool m_isUpdatePix;         // 是否更新像素值

    MySlider *m_pAccMySlider;   // 精确度滑动条
    MySlider *m_pPixMySlider;   // 像素值滑动条

    int m_pAccStep;             // 精确度步长
    int m_pPixStep;             // 像素值步长

    QLabel *m_pAccLab;          // 精确度Lab
    QLabel *m_pPixLab;          // 像素值Lab

//    QLineEdit *m_pAccLine;
//    QLineEdit *m_pPixLine;

//    QPushButton *m_pAccAddBtn;
//    QPushButton *m_pAccSubBtn;
//    QPushButton *m_pPixAddBtn;
//    QPushButton *m_pPixSubBtn;

//    QSlider *m_pAccSlider;
//    QSlider *m_pPixSlider;


    QPushButton *m_pCurvBtn;        // 曲线
    QPushButton *m_pPolyBtn;        // 多边形
    QPushButton *m_pRectBtn;        // 矩形
    QPushButton *m_pCircleBtn;      // 圆形
    QPushButton *m_pConCirBtn;      // 环形
    QPushButton *m_pMaskBtn;        // 屏蔽区
    QPushButton *m_pCopyBtn;        // 复制
    QPushButton *m_pEraseBtn;       // 擦除
    QPushButton *m_pClearBtn;       // 清除
    QPushButton *m_pPositionBtn;    // 位置

    QHBoxLayout *m_pBottomLayout;
    QGridLayout *m_pLeftWidLayout;
    QHBoxLayout *m_pRighBtnLayout;

    QWidget *m_leftWid;

};





#endif // BOTTOM_BAR
