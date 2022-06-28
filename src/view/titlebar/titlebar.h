#ifndef TITLE_BAR
#define TITLE_BAR

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QHBoxLayout>

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

private:
    void startBtnClick();
    void stopBtnClick();
    void monitorSetBtnClick();
    void sysSettingBtnClick();
    void testBtnClick();
    void NGRecordBtnClick();
    void closeBtnClick();

private:
    // 左上方标识
    QLabel *m_ptitleLab;

    // 标题栏按钮
    QPushButton *m_pStartBtn;
    QPushButton *m_pStopBtn;
    QPushButton *m_pMonitorSetBtn;
    QPushButton *m_pSysSettingBtn;
    QPushButton *m_pTestBtn;
    QPushButton *m_pNGRecordBtn;
    QPushButton *m_pCloseBtn;

    // 界面布局
    QHBoxLayout *m_pBtnLayout;

    QButtonGroup *m_pBtnGroup;
    QList<QAbstractButton *> m_pBtnList;

    bool m_pIsSetMonitor;

    // 系统设定
    SysSetting *m_pSysSetting;

    // NG记录
    NGRecord *m_pNGRecord;

    // 相机ID
    int m_cameraId;
};



#endif // TITLE_BAR
