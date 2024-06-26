﻿#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <QDebug>

#include "ngrecord.h"
#include "src/view/imgarea/imgarea.h"
#include "src/view/common/mysettings.h"

#if _MSC_VER >=1600    // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif

const QString VersionNum = "版本号: 1.1.0.0(20220801)\n";

NGRecord::NGRecord(QWidget *parent)
    : QDialog(parent)
{
    // 初始化组件
    setWidgetUi();

    // 设置组件样式
    setWidgetStyle();

    // 设置初始数据
    setData();
}

void NGRecord::setWidgetUi()
{
    m_ngTableView = new QTableView(this);
    m_ngModel = new QStandardItemModel(this);

    m_recordTextEdit = new QTextEdit(this);

    m_resultLab = new QLabel(this);

    m_resetBtn = new QPushButton(this);
    m_optRecordBtn = new QPushButton(this);
    m_closeBtn = new QPushButton(this);

    m_mainLayout  = new QHBoxLayout(this);
    m_rightLayout = new QVBoxLayout();

    m_rightLayout->addStretch();
    m_rightLayout->addWidget(m_resultLab);
    m_rightLayout->addStretch();
    m_rightLayout->addWidget(m_resetBtn);
    m_rightLayout->addWidget(m_optRecordBtn);
    m_rightLayout->addWidget(m_closeBtn);
    m_rightLayout->setContentsMargins(0, 0, 0, 0);
    m_rightLayout->setSpacing(10);

    m_mainLayout->addWidget(m_ngTableView);
    m_mainLayout->addWidget(m_recordTextEdit);
    m_mainLayout->addLayout(m_rightLayout);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(5);

    this->setLayout(m_mainLayout);

    connect(m_resetBtn,     &QPushButton::clicked, this, &NGRecord::resetBtnClick);
    connect(m_optRecordBtn, &QPushButton::clicked, this, &NGRecord::optRecordBtnClick);
    connect(m_closeBtn,     &QPushButton::clicked, this, &NGRecord::closeBtnClick);

    connect(m_ngTableView, &QTableView::clicked, this, &NGRecord::showNgRecordImg);
}

void NGRecord::setWidgetStyle()
{
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    this->setFixedSize(700, 300);
    this->setWindowTitle("NG记录");

    m_ngTableView->setFixedSize(325, this->height());
    m_recordTextEdit->setFixedSize(225, this->height());
    m_resultLab->setFixedSize(140, 30);
    m_resetBtn->setFixedSize(135, 30);
    m_optRecordBtn->setFixedSize(135, 30);
    m_closeBtn->setFixedSize(135, 30);

    QFont viewFont = this->font();
    viewFont.setPixelSize(15);
    m_ngTableView->setFont(viewFont);
    m_ngTableView->setAlternatingRowColors(true);

    m_ngModel->setHorizontalHeaderLabels({" 时间 ", " 相机 ", " 场景 ", " 结果 "});
    m_ngTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_ngTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_ngTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ngTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ngTableView->setModel(m_ngModel);
//    m_ngTableView->setFocusPolicy(Qt::NoFocus);

    m_recordTextEdit->setFont(viewFont);
    m_recordTextEdit->setReadOnly(true);

    m_resultLab->setFont(viewFont);
    m_resultLab->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    m_resultLab->setFont(viewFont);
    m_resetBtn->setFont(viewFont);
    m_optRecordBtn->setFont(viewFont);
    m_closeBtn->setFont(viewFont);

    m_resetBtn->setFocusPolicy(Qt::NoFocus);
    m_optRecordBtn->setFocusPolicy(Qt::NoFocus);
    m_closeBtn->setFocusPolicy(Qt::NoFocus);

    m_resultLab->setText("正常0次，异常5次");
    m_resetBtn->setText("清零");
    m_optRecordBtn->setText("操作记录");
    m_closeBtn->setText("关闭");

}

void NGRecord::setData()
{
    getModelData();
    m_ngTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_ngTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    QString recordStr = getNGTextRecord();
    recordStr = VersionNum + recordStr;
    m_recordTextEdit->setText(recordStr);

//    m_recordTextEdit->setText("10:40:07\n版本号:2.1.3.0(20220505)\n"
//                          "10:40:14\n相机1场景1 收到触发信号\n"
//                          "10:40:14\nCam1 Scene1\nNG, tick = 33ms\n"
//                          "10:40:14\n相机1场景1 收到触发信号\n"
//                          "10:40:14\nCam1 Scene1\nNG, tick = 33ms\n"
//                          "10:40:14\n相机1场景1 收到触发信号\n"
//                          "10:40:14\nCam1 Scene1\nNG, tick = 33ms\n"
//                          "10:40:14\n相机1场景1 收到触发信号\n"
//                          "10:40:14\nCam1 Scene1\nNG, tick = 33ms\n"
//                          "10:40:14\n相机1场景1 收到触发信号\n"
//                          "10:40:14\nCam1 Scene1\nNG, tick = 33ms\n"
//                          "10:40:14\n相机1场景1 收到触发信号\n"
//                          "10:40:14\nCam1 Scene1\nNG, tick = 33ms\n"
//                          );

    int ngTimes = MySettings::getInstance()->getValue(DetectSection, DetectNGTimes).toInt();
    int okTimes = MySettings::getInstance()->getValue(DetectSection, DetectOKTimes).toInt();

    m_resultLab->setText(QString("正常%1次，异常%2次").arg(okTimes).arg(ngTimes));
}

void NGRecord::addNgRecord(NGRecordData ngData)
{
    QList<QStandardItem *> itemList;
    itemList.append(new QStandardItem(" " + ngData.time + "     "));
    itemList.append(new QStandardItem(QString::number(ngData.cameraId)));
    itemList.append(new QStandardItem(QString::number(ngData.sceneId)));
    itemList.append(new QStandardItem(ngData.result));
    m_ngModel->appendRow(itemList);
}

void NGRecord::addNgTextRecord(QString ngText)
{
    QDateTime time  = QDateTime::currentDateTime();
    QString timeStr = time.toString("yyyy-MM-dd HH:mm:ss");

    ngText = timeStr + "\n" + ngText;

    QFile txtNgFile(MyDataBase::txtNgFilePath);
    if (txtNgFile.exists()) {
        txtNgFile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append);
        txtNgFile.write(ngText.toUtf8());
    }
    txtNgFile.close();
}

void NGRecord::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);

}

void NGRecord::resetBtnClick()
{
    OptRecord::addOptRecord("点击清零");

    // 配置文件修改
    MySettings::getInstance()->setValue(DetectSection, DetectNGTimes, "0");
    MySettings::getInstance()->setValue(DetectSection, DetectOKTimes, "0");

    m_resultLab->setText("正常0次，异常0次");
}

void NGRecord::optRecordBtnClick()
{
    OptRecord::addOptRecord("点击操作记录");

    m_optRecord = new OptRecord();
    m_optRecord->exec();
}

void NGRecord::closeBtnClick()
{
    OptRecord::addOptRecord("点击关闭");

    this->close();
}

void NGRecord::getModelData()
{
//    for (int i = 0; i < 5; i++) {
//        m_ngModel->setItem(i, 0, new QStandardItem(QString(" 2022-6-17 11:0%1:06 ").arg(i)));
//        m_ngModel->setItem(i, 1, new QStandardItem(QString(" 1 ")));
//        m_ngModel->setItem(i, 2, new QStandardItem(QString(" 1 ")));
//        m_ngModel->setItem(i, 3, new QStandardItem(QString(" 异常 ")));
//    }

    m_recordDataList = MyDataBase::getInstance()->queAllNGRecordData();

    for (int i = 0; i < m_recordDataList.size(); i++) {
        m_ngModel->setItem(i, 0, new QStandardItem(QString(" %1 ").arg(m_recordDataList[i].time)));
        m_ngModel->setItem(i, 1, new QStandardItem(QString(" %1 ").arg(m_recordDataList[i].cameraId)));
        m_ngModel->setItem(i, 2, new QStandardItem(QString(" %1 ").arg(m_recordDataList[i].sceneId)));
        m_ngModel->setItem(i, 3, new QStandardItem(QString(" %1 ").arg(m_recordDataList[i].result)));
    }
}

QString NGRecord::getNGTextRecord()
{
    QString txtNgStr;

    QFile txtNgFile(MyDataBase::txtNgFilePath);
    txtNgFile.open(QIODevice::ReadOnly | QIODevice::Text);

    txtNgStr = QString::fromUtf8(txtNgFile.readAll());

//    qDebug() << "------------txtNgStr: " << txtNgStr;

    txtNgFile.close();

    return txtNgStr;
}

void NGRecord::showNgRecordImg(const QModelIndex &index)
{
    int row = index.row();
    int cameraId = m_recordDataList[row].cameraId;
    QString ngImgPath = m_recordDataList[row].imgPath;

    m_imgAreaShowState = ImgArea::getInstance()->getShowState(cameraId);

    if (m_imgAreaShowState) {
        ImgArea::getInstance()->setShowState(false, cameraId);
    }

    ImgArea::getInstance()->clearShapes(cameraId);
    ImgArea::getInstance()->loadImage(ngImgPath, cameraId);
}
