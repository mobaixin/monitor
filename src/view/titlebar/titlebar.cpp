#include <QMessageBox>
#include <QApplication>
#include <QDebug>

#include "src/view/titlebar/titlebar.h"
#include "src/view/mainwindow.h"
#include "src/view/sidebar/sidebar.h"

#if _MSC_VER >=1600    // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif

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
    m_ptitleLab = new QLabel(this);

    m_cameraCount = MySettings::getInstance()->getValue(SysSection, CameraCountsKey).toInt();
    m_pCameraBtnGroup = new QButtonGroup(this);

    m_pAllCameraBtn = new QPushButton(this);
    m_pAllCameraBtn->setCheckable(true);

    m_pCameraBtnGroup->addButton(m_pAllCameraBtn);
    m_pCameraBtnGroup->setExclusive(true);

    for (int i = 0; i < m_cameraCount; i++) {
        m_pCameraBtnList.append(new QPushButton(this));
        m_pCameraBtnList[i]->setCheckable(true);

        m_pCameraBtnGroup->addButton(m_pCameraBtnList[i]);
    }

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
    m_pCameraBtnLayout = new QHBoxLayout();
    m_pBtnLayout = new QHBoxLayout(this);

    // 设置控件大小
    m_pAllCameraBtn->setFixedSize(50, 30);
    for (int i = 0; i < m_pCameraBtnList.size(); i++) {
        m_pCameraBtnList[i]->setFixedSize(50, 30);
    }
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
    m_pCameraBtnLayout->addWidget(m_pAllCameraBtn);
    for (int i = 0; i < m_pCameraBtnList.size(); i++) {
        m_pCameraBtnLayout->addWidget(m_pCameraBtnList[i]);
    }
    m_pCameraBtnLayout->addStretch();
    m_pCameraBtnLayout->setContentsMargins(0, 0, 0, 0);
    m_pCameraBtnLayout->setSpacing(0);

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
    m_pBtnLayout->setSpacing(6);

    m_ptitleLab->setLayout(m_pCameraBtnLayout);
    m_ptitleLab->setFixedSize(250, 30);
    m_ptitleLab->setGeometry(0, 0, 250, 30);

    this->setLayout(m_pBtnLayout);

    m_pBtnGroup->addButton(m_pStartBtn);
    m_pBtnGroup->addButton(m_pStopBtn);
    m_pBtnGroup->setExclusive(true);

    m_pStartBtn->setCheckable(true);
    m_pStopBtn->setCheckable(true);

    m_pStartBtn->setChecked(true);

    for (int i = 0; i < m_pCameraBtnList.size(); i++) {
        connect(m_pCameraBtnList[i], &QPushButton::clicked, this, &TitleBar::cameraBtnListClick);
    }

    connect(m_pAllCameraBtn,  &QPushButton::clicked, this, &TitleBar::allCameraBtnClick);
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

    if (m_pCameraBtnList.size() <= 1) {
        m_pAllCameraBtn->hide();
        m_pCameraBtnList[0]->setChecked(true);
    } else {
        m_pAllCameraBtn->setChecked(true);
//        m_pTestBtn->setDisabled(true);
    }
}

// 设置组件样式
void TitleBar::setWidgetStyle()
{
    // 设置标题栏高度
    this->setFixedHeight(30);

    // 设置按钮文字
    m_pAllCameraBtn->setText("全部");
    for (int i = 0; i < m_pCameraBtnList.size(); i++) {
        m_pCameraBtnList[i]->setText(QString("相机%1").arg(i + 1));
    }

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

//    m_ptitleLab->setStyleSheet("background:#00BFFF;color:#FFFF00;border-radius:4px;");
//    m_ptitleLab->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QPalette pal = m_pDelAlarmBtn->palette();
    pal.setColor(QPalette::Button,Qt::red);
    m_pDelAlarmBtn->setPalette(pal);
    m_pDelAlarmBtn->setAutoFillBackground(true);
    m_pDelAlarmBtn->setFlat(true);

    QString btnStyleStr = "QPushButton{background:#00BFFF;color:#000000;border-radius:5px;font-size:14px;border: 1px groove #F3F781;}"
                          "QPushButton:hover{background:#58D3F7;color:#000000;}"
                          "QPushButton:pressed{background:#00BFFF;color:#000000;}"
                          "QPushButton:disabled{background:#A9E2F3;color:#000000}";

    QString runBtnStyleStr = "QPushButton{background:#00BFFF;color:#000000;border-radius:5px;font-size:14px;border: 1px groove #F3F781;}"
                             "QPushButton:hover{background:#58D3F7;color:#000000;}"
                             "QPushButton:pressed{background:#086A87;color:#FAFAFA;}"
                             "QPushButton:checked{background:#086A87;color:#FAFAFA;}"
                             "QPushButton:disabled{background:#A9E2F3;color:#000000}";

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

    m_pAllCameraBtn->setStyleSheet(runBtnStyleStr);
    for (int i = 0; i < m_pCameraBtnList.size(); i++) {
        m_pCameraBtnList[i]->setStyleSheet(runBtnStyleStr);
    }
}

void TitleBar::setData()
{
    for (int i = 0; i < 4; i++) {
        m_detectTimeList.append(QDateTime::currentDateTime());
    }

    m_pIsSetMonitor = false;
    m_cameraId = 1;

    setAlarmBtnState(false);
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

        m_pMonitorSetBtn->setDisabled(true);
    } else {
        m_pAddMoldBtn->hide();
        m_pReDetectBtn->hide();
        m_pDelAlarmBtn->hide();

        m_pMonitorSetBtn->setDisabled(false);
    }
}

bool TitleBar::getMonitorSetState()
{
    return m_pIsSetMonitor;
}

bool TitleBar::getAllCamBtnState()
{
    if (m_pAllCameraBtn->isHidden()) {
        return false;
    }

    if (m_pAllCameraBtn->isChecked()) {
        return true;
    }

    return false;
}

void TitleBar::allCameraBtnClick()
{
    m_cameraId = 1;

//    m_pTestBtn->setDisabled(true);
    ImgArea::getInstance()->showAllCameraView();

    // 更新图形模板显示
    SideBar::getInstance()->updateShapeData();

    for(int i = 0; i < m_cameraCount; i++)//多相机NG状态显示
    {
        if(ImgArea::getInstance()->getCamNGState(i+1))
        {
             setAlarmBtnState(true);
             break;
        }
    }
}

void TitleBar::cameraBtnListClick()
{
//    m_pTestBtn->setDisabled(false);

    if (getMonitorSetState())
    {
        // 清除数据库中的图形模板
        ShapeItemData itemData;
        itemData.cameraId = m_cameraId;
        itemData.sceneId  = SideBar::getInstance()->getCurSceneID();

        MyDataBase::getInstance()->delSceneShapeItemData(itemData);

        // 将图形模板保存至数据库
        ImgArea::getInstance()->getShapeItems();
    }

    // 获取上一个相机的ID
    int prevCamId = m_cameraId;

    // 获取当前相机ID
    int i = 0;
    for (; i < m_pCameraBtnList.size(); i++) {
        if (m_pCameraBtnList[i]->isChecked()) {
            break;
        }
    }

    m_cameraId = i + 1;

    // 显示单个相机画面
    ImgArea::getInstance()->showSingleCameraView(m_cameraId);

    if (m_pIsSetMonitor) {
        // 切换相机时恢复上一个相机的视频流显示
        ImgArea::getInstance()->setShowState(true, prevCamId);
    }

    // 侧边栏数据更新
    SideBar::getInstance()->updateShowData();

    // 更新图形模板显示
    if (ImgArea::getInstance()->getCamNGState(m_cameraId) == false) {
        SideBar::getInstance()->updateShapeData();
    }

    // 处于监视设定状态
    if (m_pIsSetMonitor) {
        MainWindow::getInstance()->showMonitorSet(true, m_cameraId);
    }

    if (ImgArea::getInstance()->getCameraState(m_cameraId) == CameraState::Running) {
        m_pStartBtn->setChecked(true);
    } else {
        m_pStopBtn->setChecked(true);
    }

	//单相机NG状态显示
    if(ImgArea::getInstance()->getCamNGState(m_cameraId))
    {
        setAlarmBtnState(true);
    }
    else
    {
        setAlarmBtnState(false);
    }
}

void TitleBar::startBtnClick()
{
    ShapeItemData itemData;
    itemData.cameraId = m_cameraId;
    itemData.sceneId  = SideBar::getInstance()->getCurSceneID();
    itemData.moldId   = 1;

    if (m_pAllCameraBtn->isChecked()) {
        for (int i = 0; i < m_cameraCount; i++) {
            int cameraId = i + 1;
            itemData.cameraId = cameraId;

            ImgArea::getInstance()->setRunState(CameraState::Running, cameraId);
            ImgArea::getInstance()->startCamera(cameraId);
            ImgArea::getInstance()->loadShapeItem(itemData);
        }
    } else {
        ImgArea::getInstance()->setRunState(CameraState::Running, m_cameraId);
        ImgArea::getInstance()->startCamera(m_cameraId);
        ImgArea::getInstance()->loadShapeItem(itemData);
    }


    // 清除检测结果
    ImgArea::getInstance()->clearDetectResult();

    // 判断监视设定状态
    if (!m_pIsSetMonitor) {
        ImgArea::getInstance()->setShowState(true);
    }

    OptRecord::addOptRecord("点击开始运行");
}

void TitleBar::stopBtnClick()
{
    if (m_pAllCameraBtn->isChecked()) {
        for (int i = 0; i < m_cameraCount; i++) {
            ImgArea::getInstance()->setRunState(CameraState::Pause, i + 1);
            ImgArea::getInstance()->pauseCamera(i + 1);
        }
    } else {
        ImgArea::getInstance()->setRunState(CameraState::Pause, m_cameraId);
        ImgArea::getInstance()->pauseCamera(m_cameraId);
    }

    OptRecord::addOptRecord("点击停止运行");
}

void TitleBar::monitorSetBtnClick()
{
    // 修改监视设定状态
    m_pIsSetMonitor = (m_pIsSetMonitor == false) ? true : false;

    // 不处于监视设定状态
    if (!m_pIsSetMonitor) {
        MainWindow::getInstance()->showMonitorSet(false, m_cameraId);

        m_pMonitorSetBtn->setText("监视设定");
        m_pAllCameraBtn->setDisabled(false);
        m_pTestBtn->setDisabled(false);
        m_pNGRecordBtn->setDisabled(false);
        ImgArea::getInstance()->setShapeNoMove(true);

        // 清除数据库中的图形模板
        ShapeItemData itemData;
        itemData.cameraId = m_cameraId;
        itemData.sceneId  = SideBar::getInstance()->getCurSceneID();

        MyDataBase::getInstance()->delSceneShapeItemData(itemData);

        // 将图形模板保存至数据库
        ImgArea::getInstance()->getShapeItems();

        OptRecord::addOptRecord("点击关闭设定");

    // 处于监视设定状态
    } else {
        if (m_pAllCameraBtn->isChecked()) {
            m_pCameraBtnList[0]->click();
        }

        MainWindow::getInstance()->showMonitorSet(true, m_cameraId);

        m_pMonitorSetBtn->setText("关闭设定");
        m_pAllCameraBtn->setDisabled(true);
        m_pTestBtn->setDisabled(true);
        m_pNGRecordBtn->setDisabled(true);
        ImgArea::getInstance()->setShapeNoMove(false);

        MainWindow::getInstance()->setDetectObject();
        OptRecord::addOptRecord("点击监视设定");
    }
}

void TitleBar::sysSettingBtnClick()
{
    m_pSysSetting = new SysSetting();
    m_pSysSetting->exec();

    OptRecord::addOptRecord("点击系统设定");
}

void TitleBar::testBtnClick()
{
    OptRecord::addOptRecord("点击测试");
    NGRecord::addNgTextRecord(QString("相机%1 场景%2 手动检测").arg(m_cameraId).arg(SideBar::getInstance()->getCurSceneID()));

    if (SideBar::getInstance()->getCurSceneID() == 1) {
        SideBar::getInstance()->setCanClampMoldState(RadioBtnState::Correct);
    } else {
        SideBar::getInstance()->setCanThimbleState(RadioBtnState::Correct);
    }

    // 测试延时自动检测
//    m_detectTime = QDateTime::currentDateTime();
//    MainWindow::getInstance()->autoDetectImage(m_cameraId, SideBar::getInstance()->getCurSceneID());

    setAlarmBtnState(false);
    ImgArea::getInstance()->setShapeNoMove(true);
    ImgArea::getInstance()->clearDetectResult();

//    qDebug() << "after clearDetectResult";
//    if (ImgArea::getInstance()->getCameraState(m_cameraId) != CameraState::OffLine) {
//        m_detectTimeList[m_cameraId - 1] = QDateTime::currentDateTime();
//        ImgArea::getInstance()->detectCurImage(m_cameraId);
//        qDebug() << "3";
//    }

    if(getAllCamBtnState()) // 多相机测试
    {
        for (int i = 0; i < 4; i++) {
            if (ImgArea::getInstance()->getCameraState(i + 1) != CameraState::OffLine) {
                m_detectTimeList[i] = QDateTime::currentDateTime();
                ImgArea::getInstance()->detectCurImage(i + 1);
            }
        }
    }
    else    // 单相机测试
    {
        if (ImgArea::getInstance()->getCameraState(m_cameraId) != CameraState::OffLine)
        {
            m_detectTimeList[m_cameraId - 1] = QDateTime::currentDateTime();
            ImgArea::getInstance()->detectCurImage(m_cameraId);
        }
    }

//    qDebug() << "after detectCurImage";
}

void TitleBar::addMoldBtnClick()
{
    m_detectTimeList[m_cameraId - 1] = QDateTime::currentDateTime();

    QString fileName = m_detectTimeList[m_cameraId - 1].toString("yyyy-MM-dd-HH-mm-ss-zzz");
    QString timeStr  = m_detectTimeList[m_cameraId - 1].toString("yyyy-MM-dd HH:mm:ss");
    QString moldFilePath = QString("%1/%2.jpg").arg(MyDataBase::imgMoldFilePath).arg(fileName);
//    QString ngFilePath   = QString("%1/%2.jpg").arg(MyDataBase::imgNgFilePath).arg(fileName);

    QImage detectImage = ImgArea::getInstance()->getCurDetectImage(m_cameraId);
    detectImage.save(moldFilePath);

    SideBar::getInstance()->addAlarmImageMold(moldFilePath, timeStr);
    SideBar::getInstance()->setCanClampMoldState(RadioBtnState::Correct);
    SideBar::getInstance()->setCanThimbleState(RadioBtnState::Correct);

    setAlarmBtnState(false);

    // 保存NG图片
//    ImgArea::getInstance()->saveAsImage(ngFilePath);

    ImgArea::getInstance()->clearDetectResult();
    ImgArea::getInstance()->setShowState(true);
//    ImgArea::getInstance()->setShapeNoMove(false);

    // 添加NG记录
//    NGRecordData ngData;
//    ngData.time = timeStr;
//    ngData.cameraId = ImgArea::getInstance()->getCurDetectCameraId();
//    ngData.sceneId  = ImgArea::getInstance()->getCurDetectSceneId();
//    ngData.result   = "异常";
//    ngData.imgPath  = ngFilePath;

//    MyDataBase::getInstance()->addNGRecordData(ngData);

    OptRecord::addOptRecord("点击添加模板");
}

void TitleBar::reDetectBtnClick()
{
    ImgArea::getInstance()->clearDetectResult();

    if (ImgArea::getInstance()->getCameraState(m_cameraId) == 1) {
        m_detectTimeList[m_cameraId - 1] = QDateTime::currentDateTime();
        ImgArea::getInstance()->detectCurImage(m_cameraId);
    }

    OptRecord::addOptRecord("点击重检");
}

void TitleBar::NGRecordBtnClick()
{
    m_pNGRecord = new NGRecord();
    m_pNGRecord->exec();

    OptRecord::addOptRecord("点击NG记录");
}

void TitleBar::delAlarmBtnClick()
{
    setAlarmBtnState(false);
    ImgArea::getInstance()->clearDetectResult();

    if (ImgArea::getInstance()->getDetectSceneId() == 1) {
        SideBar::getInstance()->setCanClampMoldState(RadioBtnState::Correct);
    } else {
        SideBar::getInstance()->setCanThimbleState(RadioBtnState::Correct);
    }

    ImgArea::getInstance()->setShowState(true);
//    ImgArea::getInstance()->setShapeNoMove(false);

    OptRecord::addOptRecord("点击删除报警");
}

void TitleBar::closeBtnClick()
{
    if (!(QMessageBox::information(this,tr("提示"),tr("是否退出程序 ?"),tr("是"),tr("否"))))
    {
//       QApplication* app;
//       app->exit(0);
        MainWindow::getInstance()->close();
        MainWindow::getInstance()->deleteLater();
    }
}
