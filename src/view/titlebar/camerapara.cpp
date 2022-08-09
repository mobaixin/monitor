#include <QDebug>

#include "camerapara.h"
#include "src/view/imgarea/imgarea.h"
#include "src/view/common/mysettings.h"
#include "src/view/titlebar/optrecord.h"
#include "src/view/titlebar/titlebar.h"

#if _MSC_VER >=1600    // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif

CameraPara::CameraPara(QWidget *parent)
    : QDialog(parent)
{
    // 初始化组件
    setWidgetUi();

    // 设置组件样式
    setWidgetStyle();

    // 设置初始数据
    setData();
}

CameraPara::~CameraPara()
{

}

void CameraPara::setWidgetUi()
{
    // 初始化组件
    m_exposeTimeLab = new QLabel(this);
    m_cameraGainLab = new QLabel(this);
//    m_widDynamicLab = new QLabel(this);

    m_exposeTimeSlider = new MySlider(this);
    m_cameraGainSlider = new MySlider(this);
//    m_widDynamicSlider = new MySlider(this);

    m_defaultBtn = new QPushButton(this);
    m_cancelBtn  = new QPushButton(this);
    m_confirmBtn = new QPushButton(this);

    m_mainLayout = new QGridLayout(this);
    m_btnLayout  = new QHBoxLayout();

    // 组件布局
    m_mainLayout->addWidget(m_exposeTimeLab, 0, 0, 1, 1);
    m_mainLayout->addWidget(m_cameraGainLab, 1, 0, 1, 1);
//    m_mainLayout->addWidget(m_widDynamicLab, 2, 0, 1, 1);

    m_mainLayout->addWidget(m_exposeTimeSlider, 0, 1, 1, 2);
    m_mainLayout->addWidget(m_cameraGainSlider, 1, 1, 1, 2);
//    m_mainLayout->addWidget(m_widDynamicSlider, 2, 1, 1, 2);
    m_mainLayout->addLayout(m_btnLayout, 3, 0, 1, 3);

    m_btnLayout->addWidget(m_defaultBtn);
    m_btnLayout->addWidget(m_cancelBtn);
    m_btnLayout->addWidget(m_confirmBtn);

    m_mainLayout->setContentsMargins(5, 0, 5, 0);
    m_mainLayout->setSpacing(5);

    m_btnLayout->setContentsMargins(0, 0, 0, 0);
    m_btnLayout->setSpacing(5);

    this->setLayout(m_mainLayout);

    connect(m_defaultBtn, &QPushButton::clicked, this, &CameraPara::defaultBtnClick);
    connect(m_cancelBtn,  &QPushButton::clicked, this, &CameraPara::cancelBtnClick);
    connect(m_confirmBtn, &QPushButton::clicked, this, &CameraPara::confirmBtnClick);

}

void CameraPara::setWidgetStyle()
{
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    this->setModal(true);
    this->setFixedSize(330, 120);
    this->setWindowTitle("相机参数");

    m_exposeTimeLab->setFixedSize(85, 30);
    m_cameraGainLab->setFixedSize(85, 30);
//    m_widDynamicLab->setFixedSize(85, 30);

    m_exposeTimeSlider->setEditSize(50, 20);
    m_cameraGainSlider->setEditSize(50, 20);
//    m_widDynamicSlider->setEditSize(50, 20);

    int cameraId = TitleBar::getInstance()->getCurCameraId();

    QList<double> exposeTimeList = ImgArea::getInstance()->getExposureTime(cameraId);
    QList<int>    cameraGainList = ImgArea::getInstance()->getCameraGain(cameraId);

    m_exposeTimeSlider->setEditReadOnly(true);
    m_cameraGainSlider->setEditReadOnly(true);
//    m_widDynamicSlider->setEditReadOnly(true);

    m_exposeTimeSlider->setStep(10);
    m_cameraGainSlider->setStep(1);
//    m_widDynamicSlider->setStep(1);

    m_exposeTimeSlider->setSliderSize(100, 20);
    m_cameraGainSlider->setSliderSize(100, 20);
//    m_widDynamicSlider->setSliderSize(100, 20);

    m_exposeTimeSlider->setAutoSetEditValue(false);

    m_exposeTimeSlider->setValueRange(exposeTimeList.at(1), exposeTimeList.at(2));
    m_cameraGainSlider->setValueRange(cameraGainList.at(1), cameraGainList.at(2));
//    m_widDynamicSlider->setValueRange(0, 20);

    m_exposeTimeSlider->setValue(exposeTimeList.at(0) / exposeTimeList.at(3), false);
    m_cameraGainSlider->setValue(cameraGainList.at(0));

    this->updateExposeTime(exposeTimeList.at(0) / exposeTimeList.at(3));

    m_defaultBtn->setFixedSize(100, 30);
    m_cancelBtn->setFixedSize(100, 30);
    m_confirmBtn->setFixedSize(100, 30);

    m_exposeTimeLab->setText(QString("相机%1曝光时间:").arg(cameraId));
    m_cameraGainLab->setText(QString("相机%1增益:").arg(cameraId));
//    m_widDynamicLab->setText("相机1宽动态值:");

    m_defaultBtn->setText("默认");
    m_cancelBtn->setText("取消");
    m_confirmBtn->setText("确定");

    connect(m_exposeTimeSlider, &MySlider::valueChange, this, &CameraPara::updateExposeTime);
    connect(m_cameraGainSlider, &MySlider::valueChange, this, &CameraPara::updateCameraGain);

}

void CameraPara::setData()
{

}

void CameraPara::defaultBtnClick()
{
    OptRecord::addOptRecord("点击默认相机参数");

}

void CameraPara::cancelBtnClick()
{
    OptRecord::addOptRecord("点击取消");

    this->close();
}

void CameraPara::confirmBtnClick()
{
    OptRecord::addOptRecord("点击确定");

    ImgArea::getInstance()->saveCameraPara(TitleBar::getInstance()->getCurCameraId());
    this->close();
}

void CameraPara::updateExposeTime(int value)
{
    value = value > 0 ? value : 0;
    qDebug() << "value: " << value;

    int cameraId = TitleBar::getInstance()->getCurCameraId();
    ImgArea::getInstance()->setExposeTime(value, cameraId);

    QList<double> timeList = ImgArea::getInstance()->getExposureTime();
    double time = (value * timeList.at(3)) / 1000;
    m_exposeTimeSlider->setEditValue(QString::number(time, 'f', 2));

    MySettings::getInstance()->setValue(CameraSection, ExposeTimeKey, QString::number(value));
}

void CameraPara::updateCameraGain(int value)
{
    value = value > 0 ? value : 0;
    ImgArea::getInstance()->setCameraGain(value);

    MySettings::getInstance()->setValue(CameraSection, CameraGainKey, QString::number(value));
}
