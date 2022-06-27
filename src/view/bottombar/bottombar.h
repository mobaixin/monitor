#ifndef BOTTOM_BAR
#define BOTTOM_BAR

#include <QWidget>
#include <QGridLayout>

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

    // 获得精确度值
    int getAccuracy();

    // 获得像素值
    int getPixel();

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

private:
    MyGraphicsScene *m_pAreaScene;

    bool m_isCreatePolygon;
    bool m_isCreateCurve;
    bool m_isCreateMask;

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
