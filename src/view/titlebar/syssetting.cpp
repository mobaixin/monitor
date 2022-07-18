#include <QDebug>

#include "src/view/titlebar/syssetting.h"
#include "src/view/common/myselectframe.h"
#include "src/view/common/mysettings.h"
#include "src/view/titlebar/optrecord.h"
#include "src/view/sidebar/sidebar.h"

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
    m_prodDetectLab = new QLabel(this);

    m_moldDelaySlider = new MySlider(this);
    m_moldTimesSlider = new MySlider(this);
    m_prodDelaySlider = new MySlider(this);
    m_prodTimesSlider = new MySlider(this);

    m_prodDetectBtn = new QRadioButton(this);

    m_changeTimeBtn = new QPushButton(this);
    m_ioSettingsBtn = new QPushButton(this);
    m_cameraParaBtn = new QPushButton(this);
    m_closeSetBtn = new QPushButton(this);

    m_sysSetLayout = new QGridLayout(this);

    // 组件布局
    m_sysSetLayout->addWidget(m_moldDelayLab, 0, 0, 1, 1);
    m_sysSetLayout->addWidget(m_moldTimesLab, 1, 0, 1, 1);
    m_sysSetLayout->addWidget(m_prodDelayLab, 2, 0, 1, 1);
    m_sysSetLayout->addWidget(m_prodTimesLab, 3, 0, 1, 1);
    m_sysSetLayout->addWidget(m_prodDetectLab, 4, 0, 1, 1);

    m_sysSetLayout->addWidget(m_moldDelaySlider, 0, 1, 1, 3);
    m_sysSetLayout->addWidget(m_moldTimesSlider, 1, 1, 1, 3);
    m_sysSetLayout->addWidget(m_prodDelaySlider, 2, 1, 1, 3);
    m_sysSetLayout->addWidget(m_prodTimesSlider, 3, 1, 1, 3);
    m_sysSetLayout->addWidget(m_prodDetectBtn, 4, 1, 1, 1);

    m_sysSetLayout->addWidget(m_changeTimeBtn, 5, 0, 1, 1);
    m_sysSetLayout->addWidget(m_ioSettingsBtn, 5, 1, 1, 1);
    m_sysSetLayout->addWidget(m_cameraParaBtn, 5, 2, 1, 1);
    m_sysSetLayout->addWidget(m_closeSetBtn, 5, 3, 1, 1);

    m_sysSetLayout->setContentsMargins(0, 0, 0, 0);
    m_sysSetLayout->setSpacing(5);

    this->setLayout(m_sysSetLayout);

    connect(m_moldDelaySlider, &MySlider::valueChange, this, &SysSetting::updateMoldDelay);
    connect(m_moldTimesSlider, &MySlider::valueChange, this, &SysSetting::updateMoldTimes);
    connect(m_prodDelaySlider, &MySlider::valueChange, this, &SysSetting::updateProdDelay);
    connect(m_prodTimesSlider, &MySlider::valueChange, this, &SysSetting::updateProdTimes);

    connect(m_prodDetectBtn, &QRadioButton::clicked, this, &SysSetting::updateProdDetect);

    connect(m_changeTimeBtn, &QPushButton::clicked, this, &SysSetting::changeTimeBtnClick);
    connect(m_ioSettingsBtn, &QPushButton::clicked, this, &SysSetting::ioSettingsBtnClick);
    connect(m_cameraParaBtn, &QPushButton::clicked, this, &SysSetting::cameraParaBtnClick);
    connect(m_closeSetBtn,   &QPushButton::clicked, this, &SysSetting::closeSetBtnClick);

}

void SysSetting::setWidgetStyle()
{
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    this->setFixedSize(450, 300);
    this->setWindowTitle("系统设定");

    m_moldDelayLab->setFixedSize(100, 30);
    m_moldTimesLab->setFixedSize(100, 30);
    m_prodDelayLab->setFixedSize(100, 30);
    m_prodTimesLab->setFixedSize(100, 30);
    m_prodDetectLab->setFixedSize(100, 30);

    m_moldDelaySlider->setStep(1);
    m_moldTimesSlider->setStep(1);
    m_prodDelaySlider->setStep(1);
    m_prodTimesSlider->setStep(1);

    m_moldDelaySlider->setValueRange(0, 20);
    m_moldTimesSlider->setValueRange(0, 40);
    m_prodDelaySlider->setValueRange(0, 20);
    m_prodTimesSlider->setValueRange(0, 40);

//    m_moldDelaySlider->setValue(6);
//    m_moldTimesSlider->setValue(4);
//    m_prodDelaySlider->setValue(4);
//    m_prodTimesSlider->setValue(2);

    m_changeTimeBtn->setFixedSize(100, 30);
    m_ioSettingsBtn->setFixedSize(100, 30);
    m_cameraParaBtn->setFixedSize(100, 30);
    m_closeSetBtn->setFixedSize(100, 30);

    m_moldDelayLab->setText("检模拍照延时(秒):");
    m_moldTimesLab->setText("检模重检次数:");
    m_prodDelayLab->setText("产品拍照延时(秒):");
    m_prodTimesLab->setText("产品重检次数:");
    m_prodDetectLab->setText("产品检测:");

    m_prodDetectBtn->setText("启用");
    m_changeTimeBtn->setText("修改时间");
    m_ioSettingsBtn->setText("IO设置");
    m_cameraParaBtn->setText("相机参数");
    m_closeSetBtn->setText("关闭设定");

    m_prodDetectBtn->setChecked(true);
}

void SysSetting::setData()
{
    int moldDelay  = MySettings::getInstance()->getValue(SysSection, "moldDelay").toInt();
    int moldTimes  = MySettings::getInstance()->getValue(SysSection, "moldTimes").toInt();
    int prodDelay  = MySettings::getInstance()->getValue(SysSection, "prodDelay").toInt();
    int prodTimes  = MySettings::getInstance()->getValue(SysSection, "prodTimes").toInt();
    int prodDetect = MySettings::getInstance()->getValue(SysSection, "prodDetect").toInt();

    m_moldDelaySlider->setValue(moldDelay);
    m_moldTimesSlider->setValue(moldTimes);
    m_prodDelaySlider->setValue(prodDelay);
    m_prodTimesSlider->setValue(prodTimes);

    m_prodDetectBtn->setChecked(prodDetect);

    updateDisPlay(prodDetect);
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
    MySettings::getInstance()->setValue(SysSection, "moldDelay", QString::number(value));
}

void SysSetting::updateMoldTimes(int value)
{
    MySettings::getInstance()->setValue(SysSection, "moldTimes", QString::number(value));
}

void SysSetting::updateProdDelay(int value)
{
    MySettings::getInstance()->setValue(SysSection, "prodDelay", QString::number(value));
}

void SysSetting::updateProdTimes(int value)
{
    MySettings::getInstance()->setValue(SysSection, "prodTimes", QString::number(value));
}

void SysSetting::updateProdDetect(bool checked)
{
    int value = checked ? 1 : 0;
    MySettings::getInstance()->setValue(SysSection, "prodDetect", QString::number(value));

    SideBar::getInstance()->setDetectScene();

    updateDisPlay(checked);
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
