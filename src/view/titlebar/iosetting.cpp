#include <QDebug>

#include "iosetting.h"
#include "src/view/mainwindow.h"

#if _MSC_VER >=1600    // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif

IOSetting::IOSetting(QWidget *parent)
    : QDialog(parent)
{
    // 初始化组件
    setWidgetUi();

    // 设置组件样式
    setWidgetStyle();

    // 设置初始数据
    setData();
}

void IOSetting::setWidgetUi()
{
    // 初始化组件
    m_gridLayout1 = new QGridLayout();
    m_gridLayout2 = new QGridLayout();
    m_hboxLayout3 = new QHBoxLayout();
    m_hboxLayout4 = new QHBoxLayout();
    m_hboxLayout5 = new QHBoxLayout();
    m_hboxLayout6 = new QHBoxLayout();
    m_mainLayout  = new QVBoxLayout(this);

    MySelectFrame *myFrame;
    MySlider *mySlider;
    QLabel *myLabel;
    int frameId = 0;

    for (int i = 0; i < 3; i++) {
        myFrame = new MySelectFrame(this, frameId++, 2);
        m_frameList.append(myFrame);
        m_gridLayout1->addWidget(myFrame, i, 0, 1, 1);

        myFrame = new MySelectFrame(this, frameId++, 6);
        m_frameList.append(myFrame);
        m_gridLayout1->addWidget(myFrame, i, 1, 1, 2);

        myFrame = new MySelectFrame(this, frameId++, 2);
        m_frameList.append(myFrame);
        m_gridLayout1->addWidget(myFrame, i, 3, 1, 1);
    }

    for (int i = 0; i < 5; i++) {
        myFrame = new MySelectFrame(this, frameId++, 2);
        m_frameList.append(myFrame);
        m_gridLayout2->addWidget(myFrame, 0, i, 1, 1);
    }

    for (int i = 0; i < 2; i++) {
        myFrame = new MySelectFrame(this, frameId++, 2);
        m_frameList.append(myFrame);
        m_gridLayout2->addWidget(myFrame, 1, i, 1, 1);
    }

    myFrame = new MySelectFrame(this, frameId++, 3);
    m_frameList.append(myFrame);
    m_gridLayout2->addWidget(myFrame, 1, 2, 1, 1);

    for (int i = 0; i < 2; i++) {
        myFrame = new MySelectFrame(this, frameId++, 2);
        m_frameList.append(myFrame);
        m_gridLayout2->addWidget(myFrame, 1, 3 + i, 1, 1);
    }

    for (int i = 0; i < 3; i++) {
        myFrame = new MySelectFrame(this, frameId++, 2);
        m_frameList.append(myFrame);
        m_gridLayout2->addWidget(myFrame, 2, i, 1, 1);
    }

    myFrame = new MySelectFrame(this, frameId++, 5);
    m_frameList.append(myFrame);
    m_gridLayout2->addWidget(myFrame, 2, 3, 1, 2);

    for (int i = 0; i < 2; i++) {
        myFrame = new MySelectFrame(this, frameId++, 2);
        m_frameList.append(myFrame);
        m_gridLayout2->addWidget(myFrame, 3, i, 1, 1);
    }

    myFrame = new MySelectFrame(this, frameId++, 4);
    m_frameList.append(myFrame);
    m_gridLayout2->addWidget(myFrame, 3, 2, 1, 1);

    for (int i = 0; i < 2; i++) {
        myFrame = new MySelectFrame(this, frameId++, 2);
        m_frameList.append(myFrame);
        m_gridLayout2->addWidget(myFrame, 3, 3 + i, 1, 1);
    }

    for (int i = 0; i < 3; i++) {
        myLabel = new QLabel(this);
        mySlider = new MySlider(this);
        m_labelList.append(myLabel);
        m_sliderList.append(mySlider);
        m_hboxLayout5->addWidget(myLabel);
        m_hboxLayout5->addWidget(mySlider);
        m_hboxLayout5->addStretch();
    }
    myLabel = new QLabel(this);
    mySlider = new MySlider(this);
    m_labelList.append(myLabel);
    m_sliderList.append(mySlider);
    m_hboxLayout5->addWidget(myLabel);
    m_hboxLayout5->addWidget(mySlider);

    m_restartBtn = new QPushButton(this);
    m_passwordBtn = new QPushButton(this);
    m_ipsetBtn = new QPushButton(this);
    m_resetBtn = new QPushButton(this);
    m_testModeBtn = new QPushButton(this);
    m_saveBtn = new QPushButton(this);

    m_hboxLayout6->addWidget(m_restartBtn);
    m_hboxLayout6->addWidget(m_passwordBtn);
    m_hboxLayout6->addWidget(m_ipsetBtn);
    m_hboxLayout6->addWidget(m_resetBtn);
    m_hboxLayout6->addWidget(m_testModeBtn);
    m_hboxLayout6->addWidget(m_saveBtn);

    m_mainLayout->addLayout(m_gridLayout1);
    m_mainLayout->addLayout(m_gridLayout2);
    m_mainLayout->addLayout(m_hboxLayout3);
    m_mainLayout->addLayout(m_hboxLayout4);
    m_mainLayout->addLayout(m_hboxLayout5);
    m_mainLayout->addLayout(m_hboxLayout6);

    m_gridLayout1->setContentsMargins(0, 0, 0, 0);
    m_gridLayout2->setContentsMargins(0, 0, 0, 0);
    m_hboxLayout3->setContentsMargins(0, 0, 0, 0);
    m_hboxLayout4->setContentsMargins(0, 0, 0, 0);
    m_hboxLayout5->setContentsMargins(0, 0, 0, 0);
    m_hboxLayout6->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setContentsMargins(5, 5, 5, 5);

    m_gridLayout1->setSpacing(10);
    m_gridLayout2->setSpacing(10);
    m_hboxLayout3->setSpacing(10);
    m_hboxLayout4->setSpacing(10);
    m_hboxLayout5->setSpacing(10);
    m_hboxLayout6->setSpacing(0);
    m_mainLayout->setSpacing(20);

    this->setLayout(m_mainLayout);



    connect(m_restartBtn,  &QPushButton::clicked, this, &IOSetting::restartBtnClick);
    connect(m_passwordBtn, &QPushButton::clicked, this, &IOSetting::passwordBtnClick);
    connect(m_ipsetBtn,    &QPushButton::clicked, this, &IOSetting::ipsetBtnClick);
    connect(m_resetBtn,    &QPushButton::clicked, this, &IOSetting::resetBtnClick);
    connect(m_testModeBtn, &QPushButton::clicked, this, &IOSetting::testModeBtnClick);
    connect(m_saveBtn,     &QPushButton::clicked, this, &IOSetting::saveBtnClick);
}

void IOSetting::setWidgetStyle()
{
    this->setWindowFlags(Qt::Window);
    this->setModal(true);

    if (MainWindow::getInstance()->isFullScreen()) {
        this->showFullScreen();
    } else {
        this->setFixedSize(1350, 750);
    }

    QStringList frameNameList;
    frameNameList << "IN0信号边缘" << "OUT0提前断开(合模提前断开)" << "使用blob" <<
                     "IN1信号边缘" << "OUT1提前断开(顶针提前断开)" << "停止时断开合模" <<
                     "IN2信号边缘" << "OUT2提前断开" << "显示半成品" <<
                     "显示保护信号" << "显示保护信号重检" << "保护信号取反" << "图像校正" << "检模选信号" <<
                     "显示相机数" << "关门第二次拍" << "联合控制锁模" << "自动更新模板" << "硬件加速" <<
                     "启用全自动信号" << "显示轮廓" << "关闭错误图像动画" << "启用锁屏幕功能" <<
                     "启用白灵敏度" << "相机参数" << "模式" << "显示fps" << "几何定位";

    QStringList optionList1, optionList2, optionList3, optionList4, optionList5, optionList6;
    optionList1 << "上升沿" << "下降沿";
    optionList2 << "停用" << "IN1上升沿" << "检测前" << "IN1下降沿" << "IN0上升沿" << "IN2上升沿";
    optionList3 << "否" << "是" << "部分每次" << "完全" << "自动上锁";
    optionList4 << "停用" << "启用" << "检模NG断双路";
    optionList5 << "隐藏" << "显示";
    optionList6 << "否" << "立式机" << "OK-NG" << "OK-MODE";

    QList<QStringList> allOptionList;

    allOptionList << optionList1 << optionList2 << optionList3;
    allOptionList << optionList1 << optionList2 << optionList4;
    allOptionList << optionList1 << optionList2 << optionList3;
    allOptionList << optionList5 << optionList5 << optionList3 << optionList3 << optionList5;
    allOptionList << optionList5 << optionList3 << optionList4 << optionList4 << optionList3;
    allOptionList << optionList3 << optionList3 << optionList3 << optionList3;
    allOptionList << optionList4 << optionList3 << optionList6 << optionList3 << optionList3;

    for (int i = 0; i < m_frameList.size(); i++) {
        m_frameList.at(i)->setFrameName(frameNameList.at(i));
        m_frameList.at(i)->setOptionText(allOptionList.at(i));
    }

    QStringList labelStrList;
    labelStrList << "周期信号输出(s):" << "抗震级别:" << "最大样本数:" << "最大重检数:";
    for (int i = 0; i < m_sliderList.size(); i++) {
        m_labelList.at(i)->setText(labelStrList.at(i));
        m_labelList.at(i)->adjustSize();

        m_sliderList.at(i)->setStep(1);
        m_sliderList.at(i)->setSliderSize(120, 20);
    }

    m_sliderList[0]->setValueRange(0, 10);
    m_sliderList[1]->setValueRange(0, 5);
    m_sliderList[2]->setValueRange(0, 20);
    m_sliderList[3]->setValueRange(0, 20);

    m_restartBtn->setFixedHeight(40);
    m_passwordBtn->setFixedHeight(40);
    m_ipsetBtn->setFixedHeight(40);
    m_resetBtn->setFixedHeight(40);
    m_testModeBtn->setFixedHeight(40);
    m_saveBtn->setFixedHeight(40);

    m_restartBtn->setText("重启");
    m_passwordBtn->setText("按钮子密码");
    m_ipsetBtn->setText("相机IP配置");
    m_resetBtn->setText("恢复默认");
    m_testModeBtn->setText("测试模式");
    m_saveBtn->setText("保存");
}

void IOSetting::setData()
{
    MySettings *settings = MySettings::getInstance();
    for (int i = 0; i < m_frameList.size(); i++) {
        int num = settings->getValue(IOSetSection, FrameListKey.arg(i)).toInt();
        m_frameList[i]->setSelectNum(num);
    }

    m_sliderList[0]->setValue(settings->getValue(IOSetSection, PeriodSigKey).toInt());
    m_sliderList[1]->setValue(settings->getValue(IOSetSection, AseismicLevelKey).toInt());
    m_sliderList[2]->setValue(settings->getValue(IOSetSection, MaxSampleNumKey).toInt());
    m_sliderList[3]->setValue(settings->getValue(IOSetSection, MaxReDeteNumKey).toInt());

    for (int i = 0; i < m_frameList.size(); i++) {
        connect(m_frameList[i], &MySelectFrame::valueChange, this, &IOSetting::frameValueChange);
    }

    connect(m_sliderList[0], &MySlider::valueChange, this, &IOSetting::updatePeriodSig);
    connect(m_sliderList[1], &MySlider::valueChange, this, &IOSetting::updateAseismicLevel);
    connect(m_sliderList[2], &MySlider::valueChange, this, &IOSetting::updateMaxSampleNum);
    connect(m_sliderList[3], &MySlider::valueChange, this, &IOSetting::updateMaxReDeteNum);
}

void IOSetting::frameValueChange(int frameId, int selectNum)
{
//    qDebug() << "frameValueChange: " << selectNum;
    MySettings::getInstance()->setValue(IOSetSection, FrameListKey.arg(frameId), QString::number(selectNum));
}

void IOSetting::updatePeriodSig(int value)
{
    MySettings::getInstance()->setValue(IOSetSection, PeriodSigKey, QString::number(value));
}

void IOSetting::updateAseismicLevel(int value)
{
    MySettings::getInstance()->setValue(IOSetSection, AseismicLevelKey, QString::number(value));
}

void IOSetting::updateMaxSampleNum(int value)
{
    MySettings::getInstance()->setValue(IOSetSection, MaxSampleNumKey, QString::number(value));
}

void IOSetting::updateMaxReDeteNum(int value)
{
    MySettings::getInstance()->setValue(IOSetSection, MaxReDeteNumKey, QString::number(value));
}

void IOSetting::restartBtnClick()
{
    OptRecord::addOptRecord("点击重启");

}

void IOSetting::passwordBtnClick()
{
    OptRecord::addOptRecord("点击按钮子密码");

}

void IOSetting::ipsetBtnClick()
{
    OptRecord::addOptRecord("点击相机ip配置");

    m_cameraIpSet = new CameraIpSet();
    m_cameraIpSet->exec();
}

void IOSetting::resetBtnClick()
{
    OptRecord::addOptRecord("点击恢复默认");

    MySettings::getInstance()->setIOInitValue();
}

void IOSetting::testModeBtnClick()
{
    OptRecord::addOptRecord("点击测试模式");

}

void IOSetting::saveBtnClick()
{
    OptRecord::addOptRecord("点击保存");

    this->close();
}
