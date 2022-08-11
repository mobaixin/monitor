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

enum DetectScene {
    DetectMold = 1, // 检模
    DetectProd = 2  // 产品
};

enum RadioBtnState {
    NoState = 0,    // 无状态
    Correct = 1,    // 正确
    Wrong = 2       // 错误
};

class SideBar : public QWidget
{
    Q_OBJECT
public:
    // 单例
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

    // 添加报警模板
    void addAlarmImageMold(QString imgPath, QString timeStr);

    // 设置顶针状态
    void setThimbleState(int state);

    // 设置开模状态
    void setOpenMoldState(int state);

    // 设置可顶针状态
    void setCanThimbleState(int state);

    // 设置可合模状态
    void setCanClampMoldState(int state);

    // 设置检测场景
    void setDetectScene();

    // 更新显示数据
    void updateShowData();

    // 更新图形模板显示
    void updateShapeData();

signals:
    // 更新图形图像模板信号
    void updateShapeImgMoldSig(int cameraId, int sceneId);

private:
    void positionBtnClick();    // 位置
    void checkMoldBtnClick();   // 检模
    void productBtnClick();     // 产品
    void saveMoldBtnClick();    // 保存模板
    void addMoldBtnClick();     // 添加模板
    void homePageBtnClick();    // 首页
    void prevPageBtnClick();    // 上一页
    void nextPageBtnClick();    // 下一页
    void delMoldBtnClick();     // 删除模板
    void clearMoldBtnClick();   // 清空模板

    // 更新模板序号显示
    void updateOrderLab();

    // 切换场景时更新图形模板
    void updateSceneItemMold();

    // 加载当前的图形图片模板
    void loadCurMold();
    void loadCurImage();

    // 设置按钮的状态
    void setRadioBtnState(QRadioButton *btn, int state);
private:
    bool m_isDetectMold;    // 是否是检模
    bool m_isShowMold;      // 是否显示模板

    int m_sceneId;          // 场景ID

    int m_deteMoldNum;      // 检模模板数
    int m_prodMoldNum;      // 产品模板数

    int m_curDeteMoldIdx;   // 当前检模模板索引
    int m_curProdMoldIdx;   // 当前产品模板索引

    QRadioButton *m_thimbleBox;         // 顶针
    QRadioButton *m_openMoldBox;        // 开模
    QRadioButton *m_canThimbleBox;      // 可顶针
    QRadioButton *m_canClampMoldBox;    // 可合模

    QPushButton *m_positionBtn;     // 位置按钮
    QPushButton *m_checkMoldBtn;    // 检模按钮
    QPushButton *m_productBtn;      // 产品按钮

    QLabel *m_orderLab;             // 模板序号显示

    QPushButton *m_saveMoldBtn;     // 保存模板
    QPushButton *m_addMoldBtn;      // 添加模板
    QPushButton *m_homePageBtn;     // 首页
    QPushButton *m_prevPageBtn;     // 上一页
    QPushButton *m_nextPageBtn;     // 下一页
    QPushButton *m_delMoldBtn;      // 删除模板
    QPushButton *m_clearMoldBtn;    // 清空模板

    QVBoxLayout *m_sideBarLayout;   // 侧边栏布局

    QButtonGroup *m_btnGroup;       // 按钮组

    QList<QGraphicsPixmapItem *>  m_deteImgItemList;    // 检模图片模板列表
    QList<QList<QGraphicsItem *>> m_deteShapeItemList;  // 检模图形模板列表

    QList<QGraphicsPixmapItem *>  m_prodImgItemList;    // 产品图片模板列表
    QList<QList<QGraphicsItem *>> m_prodShapeItemList;  // 产品图形模板列表
};



#endif // SIDE_BAR
