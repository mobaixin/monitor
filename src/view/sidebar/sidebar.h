#ifndef SIDE_BAR
#define SIDE_BAR

#include <QWidget>
#include <QCheckBox>
#include <QRadioButton>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QList>

#include "src/view/imgarea/mygraphicsitem.h"
#include "src/serialport/myserialport.h"

class SideBar : public QWidget
{
    Q_OBJECT
public:
    static SideBar *getInstance(QWidget *parent = nullptr);

    explicit SideBar(QWidget *parent = nullptr);
    virtual ~SideBar();

    // 初始化组件
    void setWidgetUi();

    // 设置组件样式
    void setWidgetStyle();

    // 设置初始数据
    void setData();

    // 设置显示状态
    void setDisplayState(bool isDisplay);

    // 获取检测对象
    bool getIsDetectMold();

    // 获取场景ID
    int getCurSceneID();

    // 获取当前索引
    int getCurrentIdx();

    // 获取当前模板数
    int getCurMoldNum();

    // 保存模板
    void saveImageMold(QGraphicsPixmapItem *imgItem);
    void saveShapeMold(QList<QGraphicsItem *> itemList);

    // 获取模板
    QGraphicsPixmapItem *  getImageMold();
    QList<QGraphicsItem *> getShapeMold();

private:
    void positionBtnClick();
    void checkMoldBtnClick();
    void productBtnClick();
    void saveMoldBtnClick();
    void addMoldBtnClick();
    void homePageBtnClick();
    void prevPageBtnClick();
    void nextPageBtnClick();
    void delMoldBtnClick();
    void clearMoldBtnClick();

    void updateOrderLab();

    void loadCurMold();
    void loadCurImage();
private:
    bool m_isDetectMold;
    bool m_isShowMold;

    int m_sceneId;

    int m_deteMoldNum;
    int m_prodMoldNum;

    int m_curDeteMoldIdx;
    int m_curProdMoldIdx;

    QRadioButton *m_thimbleBox;
    QRadioButton *m_openMoldBox;
    QRadioButton *m_canThimbleBox;
    QRadioButton *m_canClampMoldBox;

    QPushButton *m_positionBtn;
    QPushButton *m_checkMoldBtn;
    QPushButton *m_productBtn;

    QLabel *m_orderLab;

    QPushButton *m_saveMoldBtn;
    QPushButton *m_addMoldBtn;
    QPushButton *m_homePageBtn;
    QPushButton *m_prevPageBtn;
    QPushButton *m_nextPageBtn;
    QPushButton *m_delMoldBtn;
    QPushButton *m_clearMoldBtn;

    QVBoxLayout *m_sideBarLayout;

    QButtonGroup *m_btnGroup;

    QList<QGraphicsPixmapItem *>  m_deteImgItemList;
    QList<QList<QGraphicsItem *>> m_deteShapeItemList;

    QList<QGraphicsPixmapItem *>  m_prodImgItemList;
    QList<QList<QGraphicsItem *>> m_prodShapeItemList;
};



#endif // SIDE_BAR
