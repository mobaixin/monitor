#ifndef TITLE_BAR
#define TITLE_BAR

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QDateTime>

#include "src/view/titlebar/syssetting.h"
#include "src/view/titlebar/ngrecord.h"

class TitleBar : public QWidget
{
    Q_OBJECT
public:
    // 单例
    static TitleBar *getInstance(QWidget *parent = nullptr);

    explicit TitleBar(QWidget *parent = 0);
    virtual ~TitleBar();

    // 初始化组件
    void setWidgetUi();

    // 设置组件样式
    void setWidgetStyle();

    // 设置初始数据
    void setData();

    // 获得当前相机ID
    int getCurCameraId();

    // 设置报警相关按钮的显示状态
    void setAlarmBtnState(bool isShow);

    // 获取当前监视状态
    bool getMonitorSetState();

    // 获取全部按钮选中状态
    bool getAllCamBtnState();

//    // 检测当前的图片 手动检测时sceneId为-1
//    int detectCurImage(int sceneId = -1, bool isShowNGRes = true);

private:
    void allCameraBtnClick();   // 全部
    void cameraBtnListClick();  // 单个相机
    void startBtnClick();       // 开始运行
    void stopBtnClick();        // 停止运行
    void monitorSetBtnClick();  // 监视设定
    void sysSettingBtnClick();  // 系统设定
    void testBtnClick();        // 测试
    void addMoldBtnClick();     // 添加模板
    void reDetectBtnClick();    // 重检
    void NGRecordBtnClick();    // NG记录
    void delAlarmBtnClick();    // 删除报警
    void closeBtnClick();       // 退出

private:
    // 相机数
    int m_cameraCount;

    // 放置所有相机按钮
    QLabel *m_ptitleLab;

    // 相机按钮
    QPushButton *m_pAllCameraBtn;
    QList<QPushButton *> m_pCameraBtnList;
    QButtonGroup *m_pCameraBtnGroup;

    // 标题栏按钮
    QPushButton *m_pStartBtn;       // 开始运行
    QPushButton *m_pStopBtn;        // 停止运行
    QPushButton *m_pMonitorSetBtn;  // 监视设定
    QPushButton *m_pSysSettingBtn;  // 系统设定
    QPushButton *m_pTestBtn;        // 测试
    QPushButton *m_pAddMoldBtn;     // 添加模板
    QPushButton *m_pReDetectBtn;    // 重检
    QPushButton *m_pNGRecordBtn;    // NG记录
    QPushButton *m_pDelAlarmBtn;    // 删除报警
    QPushButton *m_pCloseBtn;       // 退出

    // 界面布局
    QHBoxLayout *m_pCameraBtnLayout;
    QHBoxLayout *m_pBtnLayout;

    QButtonGroup *m_pBtnGroup;
    QList<QAbstractButton *> m_pBtnList;

    bool m_pIsSetMonitor;

    // 系统设定
    SysSetting *m_pSysSetting;

    // NG记录
    NGRecord *m_pNGRecord;

    // 相机ID
    int m_cameraId = 1;

    // 检测时间
    QList<QDateTime> m_detectTimeList;
};



#endif // TITLE_BAR
