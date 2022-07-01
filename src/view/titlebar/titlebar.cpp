#include <QMessageBox>
#include <QApplication>
#include <QDebug>


#include "src/view/titlebar/titlebar.h"
#include "src/view/mainwindow.h"

TitleBar *TitleBar::getInstance(QWidget *parent)
{
    static TitleBar *instance = nullptr;
    if (nullptr == instance) {
        instance = new TitleBar(parent);
    }
    return instance;
}

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent)
{
    // 初始化组件
    setWidgetUi();

    // 设置组件样式
    setWidgetStyle();

    // 设置初始数据
    setData();
}

TitleBar::~TitleBar()
{

}

// 初始化组件
void TitleBar::setWidgetUi()
{
    // 组件初始化
    m_ptitleLab      = new QLabel(this);
    m_pStartBtn      = new QPushButton(this);
    m_pStopBtn       = new QPushButton(this);
    m_pMonitorSetBtn = new QPushButton(this);
    m_pSysSettingBtn = new QPushButton(this);
    m_pTestBtn       = new QPushButton(this);
    m_pAddMoldBtn    = new QPushButton(this);
    m_pReDetectBtn   = new QPushButton(this);
    m_pNGRecordBtn   = new QPushButton(this);
    m_pDelAlarmBtn   = new QPushButton(this);
    m_pCloseBtn      = new QPushButton(this);

    m_pBtnGroup = new QButtonGroup(this);

    // 布局初始化
    m_pBtnLayout = new QHBoxLayout(this);

    // 设置控件大小
    m_ptitleLab->setFixedSize(40, 20);
    m_pStartBtn->setFixedSize(80, 30);
    m_pStopBtn->setFixedSize(80, 30);
    m_pMonitorSetBtn->setFixedSize(80, 30);
    m_pSysSettingBtn->setFixedSize(80, 30);
    m_pTestBtn->setFixedSize(80, 30);
    m_pAddMoldBtn->setFixedSize(80, 30);
    m_pReDetectBtn->setFixedSize(80, 30);
    m_pNGRecordBtn->setFixedSize(80, 30);
    m_pDelAlarmBtn->setFixedSize(80, 30);
    m_pCloseBtn->setFixedSize(80, 30);

    // 控件布局
    m_pBtnLayout->addStretch();
    m_pBtnLayout->addWidget(m_pStartBtn);
    m_pBtnLayout->addWidget(m_pStopBtn);
    m_pBtnLayout->addWidget(m_pMonitorSetBtn);
    m_pBtnLayout->addWidget(m_pSysSettingBtn);
    m_pBtnLayout->addWidget(m_pTestBtn);
    m_pBtnLayout->addWidget(m_pAddMoldBtn);
    m_pBtnLayout->addWidget(m_pReDetectBtn);
    m_pBtnLayout->addWidget(m_pNGRecordBtn);
    m_pBtnLayout->addWidget(m_pDelAlarmBtn);
    m_pBtnLayout->addWidget(m_pCloseBtn);
    m_pBtnLayout->addStretch();
    m_pBtnLayout->setContentsMargins(0, 0, 0, 0);
    m_pBtnLayout->setSpacing(10);

    m_ptitleLab->setGeometry(10, 10, 30, 15);

    this->setLayout(m_pBtnLayout);

    m_pBtnGroup->addButton(m_pStartBtn);
    m_pBtnGroup->addButton(m_pStopBtn);
    m_pBtnGroup->setExclusive(true);

    m_pStartBtn->setCheckable(true);
    m_pStopBtn->setCheckable(true);

    m_pStartBtn->setChecked(true);

    connect(m_pStartBtn,      &QPushButton::clicked, this, &TitleBar::startBtnClick);
    connect(m_pStopBtn,       &QPushButton::clicked, this, &TitleBar::stopBtnClick);
    connect(m_pMonitorSetBtn, &QPushButton::clicked, this, &TitleBar::monitorSetBtnClick);
    connect(m_pSysSettingBtn, &QPushButton::clicked, this, &TitleBar::sysSettingBtnClick);
    connect(m_pTestBtn,       &QPushButton::clicked, this, &TitleBar::testBtnClick);
    connect(m_pAddMoldBtn,    &QPushButton::clicked, this, &TitleBar::addMoldBtnClick);
    connect(m_pReDetectBtn,   &QPushButton::clicked, this, &TitleBar::reDetectBtnClick);
    connect(m_pNGRecordBtn,   &QPushButton::clicked, this, &TitleBar::NGRecordBtnClick);
    connect(m_pDelAlarmBtn,   &QPushButton::clicked, this, &TitleBar::delAlarmBtnClick);

    connect(m_pCloseBtn,      &QPushButton::clicked, this, &TitleBar::closeBtnClick);
}

// 设置组件样式
void TitleBar::setWidgetStyle()
{
    // 设置标题栏高度
    this->setFixedHeight(30);

    // 设置按钮文字
    m_pStartBtn->setText("开始运行");
    m_pStopBtn->setText("停止运行");
    m_pMonitorSetBtn->setText("监视设定");
    m_pSysSettingBtn->setText("系统设定");
    m_pTestBtn->setText("测试");
    m_pAddMoldBtn->setText("添加模板");
    m_pReDetectBtn->setText("复检");
    m_pNGRecordBtn->setText("NG记录");
    m_pDelAlarmBtn->setText("删除报警");
    m_pCloseBtn->setText("退出");

    m_ptitleLab->setStyleSheet("background:#00BFFF;color:#FFFF00;border-radius:4px;");
    m_ptitleLab->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QPalette pal = m_pDelAlarmBtn->palette();
    pal.setColor(QPalette::Button,Qt::red);
    m_pDelAlarmBtn->setPalette(pal);
    m_pDelAlarmBtn->setAutoFillBackground(true);
    m_pDelAlarmBtn->setFlat(true);

    QString btnStyleStr = "QPushButton{background:#00BFFF;color:#000000;border-radius:5px;font-size:14px;border: 1px groove #F3F781;}"
                          "QPushButton:hover{background:#58D3F7;color:#000000;}"
                          "QPushButton:pressed{background:#00BFFF;color:#000000;}";

    QString runBtnStyleStr = "QPushButton{background:#00BFFF;color:#000000;border-radius:5px;font-size:14px;border: 1px groove #F3F781;}"
                             "QPushButton:hover{background:#58D3F7;color:#000000;}"
                             "QPushButton:pressed{background:#086A87;color:#FAFAFA;}"
                             "QPushButton:checked{background:#086A87;color:#FAFAFA;}";

    QString alarmBtnStyleStr = "QPushButton{background:#FF0000;color:#000000;border-radius:5px;font-size:14px;border: 1px groove #F3F781;}"
                               "QPushButton:hover{background:#F78181;color:#000000;}"
                               "QPushButton:pressed{background:#FF0000;color:#000000;}";

    m_pStartBtn->setStyleSheet(runBtnStyleStr);
    m_pStopBtn->setStyleSheet(runBtnStyleStr);
    m_pMonitorSetBtn->setStyleSheet(btnStyleStr);
    m_pSysSettingBtn->setStyleSheet(btnStyleStr);
    m_pTestBtn->setStyleSheet(btnStyleStr);
    m_pAddMoldBtn->setStyleSheet(btnStyleStr);
    m_pReDetectBtn->setStyleSheet(btnStyleStr);
    m_pNGRecordBtn->setStyleSheet(btnStyleStr);
    m_pCloseBtn->setStyleSheet(btnStyleStr);

    m_pDelAlarmBtn->setStyleSheet(alarmBtnStyleStr);


}

void TitleBar::setData()
{
    m_pIsSetMonitor = false;
    m_cameraId = 1;

    setAlarmBtnState(false);
}

// 设置左上方标识
void TitleBar::setTitleLab(QString label)
{
    m_ptitleLab->setText(label);
}

int TitleBar::getCurCameraId()
{
    return m_cameraId;
}

void TitleBar::setAlarmBtnState(bool isShow)
{
    if (isShow) {
        m_pAddMoldBtn->show();
        m_pReDetectBtn->show();
        m_pDelAlarmBtn->show();
    } else {
        m_pAddMoldBtn->hide();
        m_pReDetectBtn->hide();
        m_pDelAlarmBtn->hide();
    }
}

void TitleBar::startBtnClick()
{
    MainWindow::getInstance()->setRunState(true);

    ImgArea::getInstance()->startCamera();
}

void TitleBar::stopBtnClick()
{
    MainWindow::getInstance()->setRunState(false);
//    MainWindow::getInstance()->setDetectObject();

    ImgArea::getInstance()->pauseCamera();
}

void TitleBar::monitorSetBtnClick()
{
    if (m_pIsSetMonitor) {
        MainWindow::getInstance()->showMonitorSet(false);
        m_pIsSetMonitor = false;

        m_pMonitorSetBtn->setText("监视设定");

    } else {
        MainWindow::getInstance()->showMonitorSet(true);
        m_pIsSetMonitor = true;

        m_pMonitorSetBtn->setText("关闭设定");
    }
}

void TitleBar::sysSettingBtnClick()
{
    m_pSysSetting = new SysSetting();
    m_pSysSetting->exec();
}

void TitleBar::testBtnClick()
{
    setAlarmBtnState(false);

    detectCurImage();
}

void TitleBar::addMoldBtnClick()
{
    QString fileName = m_detectTime.toString("yyyy-MM-dd-HH-mm-ss");
    QString timeStr  = m_detectTime.toString("yyyy-MM-dd HH:mm:ss");
    QString filePath = QString("%1/%2.png").arg(MyDataBase::imgFilePath).arg(fileName);

    SideBar::getInstance()->addAlarmImageMold(filePath, timeStr);

    setAlarmBtnState(false);
    ImgArea::getInstance()->clearDetectResult();
}

void TitleBar::reDetectBtnClick()
{
    ImgArea::getInstance()->clearDetectResult();

    detectCurImage();
}

void TitleBar::NGRecordBtnClick()
{
    m_pNGRecord = new NGRecord();
    m_pNGRecord->exec();
}

void TitleBar::delAlarmBtnClick()
{
    setAlarmBtnState(false);
    ImgArea::getInstance()->clearDetectResult();
}

void TitleBar::closeBtnClick()
{
    if (!(QMessageBox::information(this,tr("提示"),tr("是否退出程序 ?"),tr("是"),tr("否"))))
    {
//       QApplication* app;
//       app->exit(0);
        MainWindow::getInstance()->close();
    }
}

void TitleBar::detectCurImage()
{
    m_detectTime     = QDateTime::currentDateTime();
    QString fileName = m_detectTime.toString("yyyy-MM-dd-HH-mm-ss");

    QString filePath = QString("%1/%2.png").arg(MyDataBase::imgFilePath).arg(fileName);
    QImage curImg = ImgArea::getInstance()->getCurImage();
    curImg.save(filePath);

    QImage targetImg = QImage(filePath);
//    QImage targetImg = ImgArea::getInstance()->getImageItem();

    int detectRes = ImgArea::getInstance()->detectImage(targetImg);

    if (detectRes == DetectRes::NG) {
        ImgArea::getInstance()->setDetectRes(false);
        SideBar::getInstance()->setCanClampMoldState(RadioBtnState::Wrong);
        setAlarmBtnState(true);
    } else if (detectRes == DetectRes::OK) {
        ImgArea::getInstance()->setDetectRes(true);
        SideBar::getInstance()->setCanClampMoldState(RadioBtnState::Correct);
        setAlarmBtnState(false);
    } else {
        return ;
    }
}
