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
    static TitleBar *getInstance(QWidget *parent = nullptr);

    explicit TitleBar(QWidget *parent = 0);
    virtual ~TitleBar();

    // 初始化组件
    void setWidgetUi();

    // 设置组件样式
    void setWidgetStyle();

    // 设置初始数据
    void setData();

    // 设置左上方标识
    void setTitleLab(QString label);

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
    void allCameraBtnClick();
    void cameraBtnListClick();
    void startBtnClick();
    void stopBtnClick();
    void monitorSetBtnClick();
    void sysSettingBtnClick();
    void testBtnClick();
    void addMoldBtnClick();
    void reDetectBtnClick();
    void NGRecordBtnClick();
    void delAlarmBtnClick();
    void closeBtnClick();

private:
    // 左上方标识
    QLabel *m_ptitleLab;

    // 相机按钮
    int m_cameraCount;
    QPushButton *m_pAllCameraBtn;
    QList<QPushButton *> m_pCameraBtnList;
    QButtonGroup *m_pCameraBtnGroup;

    // 标题栏按钮
    QPushButton *m_pStartBtn;
    QPushButton *m_pStopBtn;
    QPushButton *m_pMonitorSetBtn;
    QPushButton *m_pSysSettingBtn;
    QPushButton *m_pTestBtn;
    QPushButton *m_pAddMoldBtn;
    QPushButton *m_pReDetectBtn;
    QPushButton *m_pNGRecordBtn;
    QPushButton *m_pDelAlarmBtn;
    QPushButton *m_pCloseBtn;

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
