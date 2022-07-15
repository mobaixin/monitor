#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFont>
#include <QLabel>
#include <QWidget>
#include <QListWidget>
#include <QStackedWidget>
#include <QRegExp>
#include <QTime>
#include <QVector>
#include <QPoint>
#include <QEvent>

#include "src/view/titlebar/titlebar.h"
#include "src/view/bottombar/bottombar.h"
#include "src/view/sidebar/sidebar.h"
#include "src/view/imgarea/imgarea.h"
#include "src/database/mydatabase.h"
#include "src/view/common/mysettings.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    static MainWindow *getInstance();
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 初始化组件
    void setWidgetUi();

    // 设置组件样式
    void setWidgetStyle();

    // 设置初始数据
    void setData();

    void closeEvent(QCloseEvent *);

    // 设置运行状态
    void setRunState(int state);

    // 设置显示状态
    void showMonitorSet(bool isDisplay);

    // 获取检测对象
    void setDetectObject();

    // 保存模板
    void saveMold();

    // 加载模板
    void loadMold();

    // 调整底边栏位置
    void adjustBottomBarPos();

    // 自动检测
    int autoDetectImage(int sceneId);

public:
    int status;

private:
    int init_SDK();

private:
    // 标题栏
    TitleBar *m_pTitleBar;

    // 图像区域
    ImgArea *m_pImgArea;

    // 底边栏
    BottomBar *m_pBottomBar;

    // 侧边栏
    SideBar *m_pSideBar;

    // 总体布局
    QVBoxLayout *m_pMainLayout;
    QWidget *m_pMainWid;
};
#endif // MAINWINDOW_H
