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

    void circleBtnClick();
    void ellipseBtnClick();
    void conCircleBtnClick();
    void rectBtnClick();
    void polygonBtnClick();
    void curveBtnClick();
    void maskBtnClick();
    void CopyBtnClick();
    void positionBtnClick();

    void updateItemAcc(int acc);
    void updateItemPix(int pix);

private:
    MyGraphicsScene *m_pAreaScene;

    MyRectangle *m_newMyRect;
    MyCircle *m_newMyCircle;
    MyConcentricCircle *m_newMyConCircle;

    bool m_isCreatePolygon;
    bool m_isCreateCurve;
    bool m_isCreateMask;

    bool m_isUpdateAcc;
    bool m_isUpdatePix;

    MySlider *m_pAccMySlider;
    MySlider *m_pPixMySlider;

    int m_pAccStep;
    int m_pPixStep;

    QLabel *m_pAccLab;
    QLabel *m_pPixLab;

//    QLineEdit *m_pAccLine;
//    QLineEdit *m_pPixLine;

//    QPushButton *m_pAccAddBtn;
//    QPushButton *m_pAccSubBtn;
//    QPushButton *m_pPixAddBtn;
//    QPushButton *m_pPixSubBtn;

//    QSlider *m_pAccSlider;
//    QSlider *m_pPixSlider;


    QPushButton *m_pCurvBtn;
    QPushButton *m_pPolyBtn;
    QPushButton *m_pRectBtn;
    QPushButton *m_pCircleBtn;
    QPushButton *m_pConCirBtn;
    QPushButton *m_pMaskBtn;
    QPushButton *m_pCopyBtn;
    QPushButton *m_pEraseBtn;
    QPushButton *m_pClearBtn;
    QPushButton *m_pPositionBtn;

    QHBoxLayout *m_pBottomLayout;
    QGridLayout *m_pLeftWidLayout;
    QHBoxLayout *m_pRighBtnLayout;

    QWidget *m_leftWid;

};





#endif // BOTTOM_BAR
