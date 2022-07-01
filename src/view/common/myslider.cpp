#include "myslider.h"

MySlider::MySlider(QWidget *parent)
    : QWidget(parent)
{
    // 初始化组件
    setWidgetUi();

    // 设置组件样式
    setWidgetStyle();

    // 设置初始数据
    setData();
}

MySlider::~MySlider()
{

}

// 初始化组件
void MySlider::setWidgetUi()
{
    m_lineEdit = new QLineEdit(this);

    m_addBtn = new QPushButton(this);
    m_subBtn = new QPushButton(this);

    m_slider = new QSlider(this);

    m_slideLayout = new QHBoxLayout(this);

    m_slideLayout->addWidget(m_lineEdit);
    m_slideLayout->addWidget(m_subBtn);
    m_slideLayout->addWidget(m_slider);
    m_slideLayout->addWidget(m_addBtn);
    m_slideLayout->setContentsMargins(0, 0, 0, 0);
    m_slideLayout->setSpacing(5);

    this->setLayout(m_slideLayout);

    m_sliderStep = 1;
    m_slider->setOrientation(Qt::Horizontal);
    m_slider->setMinimum(0);
    m_slider->setMaximum(255);
    m_slider->setSingleStep(m_sliderStep);
    m_slider->setPageStep(m_sliderStep);
    m_slider->setTracking(true);

    m_lineEdit->setText(QString::number(m_slider->value()));

    //启用长按
    m_addBtn->setAutoRepeat(true);
    m_addBtn->setAutoRepeatDelay(400);
    m_addBtn->setAutoRepeatInterval(20);
    m_subBtn->setAutoRepeat(true);
    m_subBtn->setAutoRepeatDelay(400);
    m_subBtn->setAutoRepeatInterval(20);

    connect(m_addBtn, &QPushButton::clicked, this, &MySlider::addBtnClick);
    connect(m_subBtn, &QPushButton::clicked, this, &MySlider::subBtnClick);

    connect(m_slider, &QSlider::valueChanged, this, &MySlider::setLineValue);
    connect(m_lineEdit, &QLineEdit::editingFinished, this, &MySlider::setSliderValue);
}

// 设置组件样式
void MySlider::setWidgetStyle()
{
    m_lineEdit->setFixedSize(40, 20);

    m_addBtn->setFixedSize(30, 30);
    m_subBtn->setFixedSize(30, 30);

    m_slider->setFixedSize(200, 20);

    m_addBtn->setText("+");
    m_subBtn->setText("-");

    QFont btnFont = this->font();
    btnFont.setPixelSize(20);
    m_addBtn->setFont(btnFont);

    m_lineEdit->setFocusPolicy(Qt::NoFocus);
    m_addBtn->setFocusPolicy(Qt::NoFocus);
    m_subBtn->setFocusPolicy(Qt::NoFocus);
}

// 设置初始数据
void MySlider::setData()
{
    m_isAutoSetEditValue = true;
}

void MySlider::setStep(int step)
{
    m_sliderStep = step;
    m_slider->setSingleStep(m_sliderStep);
    m_slider->setPageStep(m_sliderStep);
}

void MySlider::setValueRange(int min, int max)
{
    m_slider->setMinimum(min);
    m_slider->setMaximum(max);

}

void MySlider::setEditSize(int width, int height)
{
    m_lineEdit->setFixedSize(width, height);
}

QString MySlider::getEditValue()
{
    return m_lineEdit->text();
}

void MySlider::setEditReadOnly(bool isReadOnly)
{
    m_lineEdit->setReadOnly(isReadOnly);
}

void MySlider::setSliderSize(int width, int height)
{
    m_slider->setFixedSize(width, height);
}

void MySlider::setAutoSetEditValue(bool isAuto)
{
    m_isAutoSetEditValue = isAuto;
    if (!isAuto) {
        disconnect(m_lineEdit, &QLineEdit::editingFinished, this, &MySlider::setSliderValue);
    }
}

void MySlider::setValue(int value, bool isSetEdit)
{
    m_slider->setValue(value);

    if (isSetEdit) {
        m_lineEdit->setText(QString::number(value));
    }
}

void MySlider::setEditValue(QString value)
{
    m_lineEdit->setText(value);
}

void MySlider::addBtnClick()
{
    m_slider->setValue(m_slider->value() + m_sliderStep);
}

void MySlider::subBtnClick()
{
    m_slider->setValue(m_slider->value() - m_sliderStep);
}

void MySlider::setLineValue(int value)
{
    if (m_isAutoSetEditValue) {
        m_lineEdit->setText(QString::number(value));
    }

    emit valueChange(value);
}

void MySlider::setSliderValue()
{
    if (!m_isAutoSetEditValue) {
        int value = m_lineEdit->text().toInt();
        m_slider->setValue(value);
    }
}
