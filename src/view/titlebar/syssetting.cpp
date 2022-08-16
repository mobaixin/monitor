#include <QDebug>

#include "src/view/titlebar/syssetting.h"
#include "src/view/common/myselectframe.h"
#include "src/view/common/mysettings.h"
#include "src/view/titlebar/optrecord.h"
#include "src/view/sidebar/sidebar.h"
#include "src/view/imgarea/imgarea.h"

#if _MSC_VER >=1600    // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif

SysSetting::SysSetting(QWidget *parent)
    : QDialog(parent)
{
    // 初始化组件
    setWidgetUi();

    // 设置组件样式
    setWidgetStyle();

    // 设置初始数据
    setData();
}

SysSetting::~SysSetting()
{

}

void SysSetting::setWidgetUi()
{
    // 初始化组件
    m_moldDelayLab = new QLabel(this);
    m_moldTimesLab = new QLabel(this);
    m_prodDelayLab = new QLabel(this);
    m_prodTimesLab = new QLabel(this);
    m_prodDetectLab  = new QLabel(this);
    m_cameraCountLab = new QLabel(this);

    m_moldDelaySlider = new MySlider(this);
    m_moldTimesSlider = new MySlider(this);
    m_prodDelaySlider = new MySlider(this);
    m_prodTimesSlider = new MySlider(this);

    m_prodDetectBtn = new QRadioButton(this);

    m_cameraBtnGroup = new QButtonGroup(this);
    m_cameraBtnGroup->setExclusive(true);

    for (int i = 0; i < 4; i++) {
        m_cameraBtnList.append(new QPushButton(this));
        m_cameraBtnList[i]->setCheckable(true);
        m_cameraBtnGroup->addButton(m_cameraBtnList[i]);
    }

    m_changeTimeBtn = new QPushButton(this);
    m_ioSettingsBtn = new QPushButton(this);
    m_cameraParaBtn = new QPushButton(this);
    m_closeSetBtn = new QPushButton(this);

    m_sysSetLayout = new QGridLayout();
    m_camBtnLayout = new QHBoxLayout();
    m_btnLayout    = new QHBoxLayout();
    m_mainLayout   = new QVBoxLayout(this);

    // 组件布局
    m_sysSetLayout->addWidget(m_moldDelayLab, 0, 0, 1, 1);
    m_sysSetLayout->addWidget(m_moldTimesLab, 1, 0, 1, 1);
    m_sysSetLayout->addWidget(m_prodDelayLab, 2, 0, 1, 1);
    m_sysSetLayout->addWidget(m_prodTimesLab, 3, 0, 1, 1);
    m_sysSetLayout->addWidget(m_prodDetectLab, 4, 0, 1, 1);
    m_sysSetLayout->addWidget(m_cameraCountLab, 5, 0, 1, 1);

    m_sysSetLayout->addWidget(m_moldDelaySlider, 0, 1, 1, 3);
    m_sysSetLayout->addWidget(m_moldTimesSlider, 1, 1, 1, 3);
    m_sysSetLayout->addWidget(m_prodDelaySlider, 2, 1, 1, 3);
    m_sysSetLayout->addWidget(m_prodTimesSlider, 3, 1, 1, 3);
    m_sysSetLayout->addWidget(m_prodDetectBtn, 4, 1, 1, 1);
    m_sysSetLayout->addLayout(m_camBtnLayout, 5, 1, 1, 3);

    m_sysSetLayout->setContentsMargins(5, 5, 5, 5);
    m_sysSetLayout->setSpacing(20);

    for (int i = 0; i < m_cameraBtnList.size(); i++) {
        m_camBtnLayout->addWidget(m_cameraBtnList[i]);
    }
    m_camBtnLayout->setContentsMargins(0, 0, 0, 0);
    m_camBtnLayout->setSpacing(5);

    m_btnLayout->addWidget(m_changeTimeBtn);
    m_btnLayout->addWidget(m_ioSettingsBtn);
    m_btnLayout->addWidget(m_cameraParaBtn);
    m_btnLayout->addWidget(m_closeSetBtn);

    m_btnLayout->setContentsMargins(0, 0, 0, 0);
    m_btnLayout->setSpacing(5);

    m_mainLayout->addLayout(m_sysSetLayout);
    m_mainLayout->addStretch();
    m_mainLayout->addLayout(m_btnLayout);

    m_mainLayout->setContentsMargins(5, 5, 5, 5);
    m_mainLayout->setSpacing(5);

    this->setLayout(m_mainLayout);

    if (ImgArea::getInstance()->getCameraCounts() == 0) {
        m_cameraParaBtn->hide();
    }

    connect(m_moldDelaySlider, &MySlider::valueChange, this, &SysSetting::updateMoldDelay);
    connect(m_moldTimesSlider, &MySlider::valueChange, this, &SysSetting::updateMoldTimes);
    connect(m_prodDelaySlider, &MySlider::valueChange, this, &SysSetting::updateProdDelay);
    connect(m_prodTimesSlider, &MySlider::valueChange, this, &SysSetting::updateProdTimes);

    connect(m_prodDetectBtn, &QRadioButton::clicked, this, &SysSetting::updateProdDetect);

    connect(m_changeTimeBtn, &QPushButton::clicked, this, &SysSetting::changeTimeBtnClick);
    connect(m_ioSettingsBtn, &QPushButton::clicked, this, &SysSetting::ioSettingsBtnClick);
    connect(m_cameraParaBtn, &QPushButton::clicked, this, &SysSetting::cameraParaBtnClick);
    connect(m_closeSetBtn,   &QPushButton::clicked, this, &SysSetting::closeSetBtnClick);

    for (int i = 0; i < m_cameraBtnList.size(); i++) {
        connect(m_cameraBtnList[i], &QPushButton::clicked, this, &SysSetting::updateCameraCount);
    }
}

void SysSetting::setWidgetStyle()
{
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    this->setFixedSize(450, 330);
    this->setWindowTitle("系统设定");

    m_moldDelayLab->setFixedSize(100, 30);
    m_moldTimesLab->setFixedSize(100, 30);
    m_prodDelayLab->setFixedSize(100, 30);
    m_prodTimesLab->setFixedSize(100, 30);
    m_prodDetectLab->setFixedSize(100, 30);
    m_cameraCountLab->setFixedSize(100, 30);

    m_moldDelaySlider->setStep(1);
    m_moldTimesSlider->setStep(1);
    m_prodDelaySlider->setStep(1);
    m_prodTimesSlider->setStep(1);

//    m_moldDelaySlider->setValue(6);
//    m_moldTimesSlider->setValue(4);
//    m_prodDelaySlider->setValue(4);
//    m_prodTimesSlider->setValue(2);

    for (int i = 0; i < m_cameraBtnList.size(); i++) {
        m_cameraBtnList[i]->setFixedHeight(30);
        m_cameraBtnList[i]->setFocusPolicy(Qt::NoFocus);
    }

    m_changeTimeBtn->setFixedHeight(30);
    m_ioSettingsBtn->setFixedHeight(30);
    m_cameraParaBtn->setFixedHeight(30);
    m_closeSetBtn->setFixedHeight(30);

    m_moldDelayLab->setText("检模拍照延时(秒):");
    m_moldTimesLab->setText("检模重检次数:");
    m_prodDelayLab->setText("产品拍照延时(秒):");
    m_prodTimesLab->setText("产品重检次数:");
    m_prodDetectLab->setText("产品检测:");
    m_cameraCountLab->setText("相机数:");

    for (int i = 0; i < m_cameraBtnList.size(); i++) {
        m_cameraBtnList[i]->setText(QString("%1个").arg(i + 1));
    }

    m_prodDetectBtn->setText("启用");
    m_changeTimeBtn->setText("修改时间");
    m_ioSettingsBtn->setText("IO设置");
    m_cameraParaBtn->setText("相机参数");
    m_closeSetBtn->setText("关闭设定");

    m_prodDetectBtn->setChecked(true);
}

void SysSetting::setData()
{
    // 获取最大重检次数
    int maxDetectTimes = MySettings::getInstance()->getValue(IOSetSection, MaxReDeteNumKey).toInt();

    m_moldDelaySlider->setValueRange(0, 20);
    m_moldTimesSlider->setValueRange(0, maxDetectTimes);
    m_prodDelaySlider->setValueRange(0, 20);
    m_prodTimesSlider->setValueRange(0, maxDetectTimes);

    // 获取延时和重检次数
    int moldDelay  = MySettings::getInstance()->getValue(SysSection, MoldDelayKey).toInt();
    int moldTimes  = MySettings::getInstance()->getValue(SysSection, MoldTimesKey).toInt();
    int prodDelay  = MySettings::getInstance()->getValue(SysSection, ProdDelayKey).toInt();
    int prodTimes  = MySettings::getInstance()->getValue(SysSection, ProdTimesKey).toInt();

    int prodDetect = MySettings::getInstance()->getValue(SysSection, ProdDetectKey).toInt();
    int cameraPara = MySettings::getInstance()->getValue(IOSetSection, FrameListKey.arg(24)).toInt();

    m_moldDelaySlider->setValue(moldDelay);
    m_moldTimesSlider->setValue(moldTimes);
    m_prodDelaySlider->setValue(prodDelay);
    m_prodTimesSlider->setValue(prodTimes);

    m_prodDetectBtn->setChecked(prodDetect);

    updateDisPlay(prodDetect);

    if (cameraPara == 0) {
        m_cameraParaBtn->hide();
    } else {
        m_cameraParaBtn->show();
    }

    // 获得相机数
    int showCamCount = MySettings::getInstance()->getValue(IOSetSection, FrameListKey.arg(14)).toInt();
    int cameraCount  = MySettings::getInstance()->getValue(SysSection, CameraCountsKey).toInt();

    if (showCamCount == 0) {
        m_cameraCountLab->hide();
        for (int i = 0; i < m_cameraBtnList.size(); i++) {
            m_cameraBtnList[i]->hide();
        }
        this->setFixedHeight(300);
    } else {
        m_cameraCountLab->show();
        for (int i = 0; i < m_cameraBtnList.size(); i++) {
            m_cameraBtnList[i]->show();
        }
        m_cameraBtnList[cameraCount - 1]->setChecked(true);
        this->setFixedHeight(330);
    }
}

void SysSetting::changeTimeBtnClick()
{
    OptRecord::addOptRecord("点击修改时间");

}

void SysSetting::ioSettingsBtnClick()
{
    OptRecord::addOptRecord("点击IO设置");

    m_ioSetting = new IOSetting();
    m_ioSetting->exec();

    setData();
}

void SysSetting::cameraParaBtnClick()
{
    OptRecord::addOptRecord("点击相机参数");

    m_cameraPara = new CameraPara();
    m_cameraPara->exec();
}

void SysSetting::closeSetBtnClick()
{
    OptRecord::addOptRecord("点击关闭");

    this->close();
}

void SysSetting::updateMoldDelay(int value)
{
    MySettings::getInstance()->setValue(SysSection, MoldDelayKey, QString::number(value));
}

void SysSetting::updateMoldTimes(int value)
{
    MySettings::getInstance()->setValue(SysSection, MoldTimesKey, QString::number(value));
}

void SysSetting::updateProdDelay(int value)
{
    MySettings::getInstance()->setValue(SysSection, ProdDelayKey, QString::number(value));
}

void SysSetting::updateProdTimes(int value)
{
    MySettings::getInstance()->setValue(SysSection, ProdTimesKey, QString::number(value));
}

void SysSetting::updateProdDetect(bool checked)
{
    int value = checked ? 1 : 0;
    MySettings::getInstance()->setValue(SysSection, ProdDetectKey, QString::number(value));

    SideBar::getInstance()->setDetectScene();

    updateDisPlay(checked);
}

void SysSetting::updateCameraCount()
{
    int i = 0;
    for (; i < m_cameraBtnList.size(); i++) {
        if (m_cameraBtnList[i]->isChecked()) {
            break;
        }
    }
    MySettings::getInstance()->setValue(SysSection, CameraCountsKey, QString::number(i + 1));
}

void SysSetting::updateDisPlay(bool isShowProd)
{
    if (isShowProd) {
        m_prodDelayLab->show();
        m_prodTimesLab->show();
        m_prodDelaySlider->show();
        m_prodTimesSlider->show();
    } else {
        m_prodDelayLab->hide();
        m_prodTimesLab->hide();
        m_prodDelaySlider->hide();
        m_prodTimesSlider->hide();
    }
}
