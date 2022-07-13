#include "myselectframe.h"

MySelectFrame::MySelectFrame(QWidget *parent, int frameId, int btnNum)
    : QWidget(parent), m_frameId(frameId), m_radioBtnNum(btnNum)
{
    // 初始化组件
    setWidgetUi();

    // 设置组件样式
    setWidgetStyle();

    // 设置初始数据
    setData();
}

void MySelectFrame::setWidgetUi()
{
    // 初始化组件
    m_frame = new QFrame(this);
    m_frameNameLab = new QLabel(this);
    m_frameLayout = new QHBoxLayout();
    m_mainLayout  = new QHBoxLayout(this);

    QRadioButton *radioBtn;
    for (int i = 0; i < m_radioBtnNum; i++) {
        radioBtn = new QRadioButton(this);
        m_radioBtnList.append(radioBtn);
        m_frameLayout->addWidget(radioBtn);
        connect(radioBtn, &QRadioButton::clicked, this, &MySelectFrame::radioButtonClick);
    }

    m_frameLayout->setContentsMargins(5, 5, 5, 5);
    m_frameLayout->setSpacing(5);

    m_mainLayout->addWidget(m_frame);
    m_mainLayout->setContentsMargins(0, 6, 0, 0);
    m_mainLayout->setSpacing(0);

    m_frame->setLayout(m_frameLayout);
    this->setLayout(m_mainLayout);
}

void MySelectFrame::setWidgetStyle()
{
    this->resize(300, 150);
    this->setFixedHeight(80);

    m_frame->setObjectName("MyFrame");
    m_frame->setStyleSheet("#MyFrame{"
                           "border-radius:2px;"
                           "border:1px solid rgb(0, 0, 0)"
                           "}");

    m_frameNameLab->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_frameNameLab->setText("测试");
    m_frameNameLab->adjustSize();
    m_frameNameLab->setGeometry(20, 0, m_frameNameLab->width() + 10, m_frameNameLab->height());
    m_frameNameLab->setAutoFillBackground(true);
    m_frameNameLab->setBackgroundRole(QPalette::Window);

    for (int i = 0; i < m_radioBtnNum; i++) {
        m_radioBtnList.at(i)->setText("测试");
    }
}

void MySelectFrame::setData()
{
    m_selectNum = 0;
//    m_radioBtnList.at(m_selectNum)->setChecked(true);
}

void MySelectFrame::setFrameName(QString name)
{
    m_frameNameLab->setText(name);
    m_frameNameLab->adjustSize();
    m_frameNameLab->resize(m_frameNameLab->width() + 10, m_frameNameLab->height());
}

void MySelectFrame::setOptionText(QStringList btnNameList)
{
    for (int i = 0; i < m_radioBtnList.size(); i++) {
        if (i < btnNameList.size()) {
            m_radioBtnList.at(i)->setText(btnNameList.at(i));
        }
    }
}

void MySelectFrame::setSelectNum(int num)
{
    m_selectNum = num;
    m_radioBtnList.at(m_selectNum)->setChecked(true);

    emit valueChange(m_frameId, m_selectNum);
}

int MySelectFrame::getSelectNum()
{
    return m_selectNum;
}

void MySelectFrame::radioButtonClick()
{
    for (int i = 0; i < m_radioBtnList.size(); i++) {
        if (m_radioBtnList[i]->isChecked()) {
            setSelectNum(i);
            break;
        }
    }
}
