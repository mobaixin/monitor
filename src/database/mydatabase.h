﻿#ifndef MYDATABASE_H
#define MYDATABASE_H

#include <QSqlDatabase>
#include <QObject>
#include <QFile>
#include <QList>
#include <QString>
#include <QThread>
#include <QMutexLocker>
#include <QApplication>
#include <QDir>

const QString SHAPEITEM = "shape_item";     // 图形模板表
const QString IMGMOLD   = "image_mold";     // 图像模板表
const QString NGRECORD  = "ng_record";      // NG记录表
const QString OPTRECORD = "opt_record";     // 操作记录表
const QString CAMERAIPDEPLOY = "camera_ip_deploy";  // 相机IP配置
const QString DBVERSION = "db_version";     // 数据库版本

enum DB_RETURN_STATUS{              //数据库操作结果返回表
    DB_OP_SUCC          = (0),      //数据库操作成功
    DB_UNCONNECT        = (-1),     //数据库连接失败导致的错误
    INVALID_INPUT       = (-2),     //无效的输入值
    DB_DISORDERD        = (-3),     //数据库与外界顺序不符导致的失败
    DB_EMPTY_TABLE      = (-4),     //数据库中的表格内容为空
    DB_OP_ADD_FAILED    = (-5),     //数据库添加操作失败
    DB_OP_DEL_FAILED    = (-6),     //数据库删除操作失败
    DB_OP_GET_FAILED    = (-7),     //数据库查询操作失败
    DB_OP_ADD_REPEAT    = (-8),     //数据库添加失败，重复添加
    DB_INSIDE_FAILED    = (-9),     //数据库内部错误
    CREATE_TABLE_FAILED = (-10),    //数据库创表失败
    DEL_TABLE_FAILED    = (-11),    //数据库删表失败
};

// 图形模板数据
typedef struct ShapeItemData{
    int cameraId;       // 相机ID
    int sceneId;        // 场景ID
    int moldId;         // 模板ID(未使用)
    int itemId;         // 图形ID(未使用)
    int type;           // 图形类型
    QString center;     // 中心点坐标
    QString edge;       // 边缘点坐标
    QString boundRect;  // 外接矩形的宽度和高度
    QString pointList;  // 边缘点坐标列表
    int accuracy;       // 精确度
    int pixel;          // 像素范围
} ShapeItemData;

// 图像模板数据
typedef struct ImageMoldData{
    int cameraId;       // 相机ID
    int sceneId;        // 场景ID
    int moldId;         // 图片模板ID
    QString imgPath;    // 图片路径
    QString imgContent; // 图片内容(未使用)
    QString time;       // 时间
} ImageMoldData;

// NG记录数据
typedef struct NGRecordData{
    QString time;       // 时间
    int cameraId;       // 相机ID
    int sceneId;        // 场景ID
    QString result;     // 检测结果
    QString imgPath;    // NG图片路径
} NGRecordData;

// 操作记录数据
typedef struct OptRecordData{
    int recordId;       // 操作记录ID
    QString time;       // 操作时间
    QString optorName;  // 操作员
    QString optLog;     // 操作内容
} OptRecordData;

// 相机IP数据
typedef struct CameraIPData {
    int cameraId;           // 相机ID
    QString serialId;       // 序列号
    QString nickName;       // 自定义名称
    QString portIp;         // 接口IP(本机网卡IP)
    QString state;          // 相机状态
    QString cameraIp;       // 相机IP
    QString cameraMask;     // 相机掩码
    QString cameraGateway;  // 相机网关
} CameraIPData;

// 数据库版本数据
typedef struct DBVersionData{
    QString versionId;  // 版本ID
    QString time;       // 时间
} DBVersionData;

class MyDataBase : public QObject
{
    Q_OBJECT
public:
    //单例，初始化返回指针，完成数据库文件建立
    static MyDataBase* getInstance();
    explicit MyDataBase(QObject *parent = nullptr);
    ~MyDataBase();

    // open数据库
    int initDataBase();

    // -----ShapeItem表操作-----
    int addShapeItemData(ShapeItemData itemData);
    int delShapeItemData(ShapeItemData itemData);
    QList<ShapeItemData> queShapeItemData(ShapeItemData itemData);
    int altShapeItemData(ShapeItemData itemData);

    // 清除当前场景的所有图形
    int delSceneShapeItemData(ShapeItemData itemData);
    // 获取当前场景中图形的数量
    int getMoldNum(ShapeItemData itemData);
    // 更新图形的模板ID
    int updateItemMoldId(ShapeItemData itemData);


    // -----ImgMold表操作-----
    int addImgMoldData(ImageMoldData imgData);
    int delImgMoldData(ImageMoldData imgData);
    ImageMoldData queImgMoldData(ImageMoldData imgData);
    int altImgMoldData(ImageMoldData imgData);

    // 获取当前场景的所有图片模板
    QList<ImageMoldData> queAllImgMoldData(ImageMoldData imgData);
    // 更新图片的模板ID
    int updateImgMoldId(ImageMoldData imgData);
    // 清除当前场景的所有图片模板
    int delSceneImgMoldData(ImageMoldData imgData);
    // 获取当前场景中图片模板的数量
    int getImageMoldNum(ImageMoldData imgData);


    // -----NGRecord表操作-----
    int addNGRecordData(NGRecordData recordData);
    int delNGRecordData(NGRecordData recordData);
    NGRecordData queNGRecordData(NGRecordData recordData);
    int altNGRecordData(NGRecordData recordData);

    // 获取所有的NG记录
    QList<NGRecordData> queAllNGRecordData();


    // -----OptRecord表操作-----
    int addOptRecordData(OptRecordData recordData);
    int delOptRecordData(OptRecordData recordData);
    OptRecordData queOptRecordData(OptRecordData recordData);
//    int altOptRecordData(OptRecordData recordData);

    // 获取所有的操作记录
    QList<OptRecordData> queAllOptRecordData();


    // -----CameraIPDeploy表操作-----
    int addCameraIPData(CameraIPData cameraIPData);
    int delCameraIPData(CameraIPData cameraIPData);
    CameraIPData queCameraIPData(CameraIPData cameraIPData);
    int altCameraIPData(CameraIPData cameraIPData);

    // 获取所有的记录
    QList<CameraIPData> queAllCameraIPData();


    // -----DBVersion表操作-----
    int addDBVersionData(DBVersionData dBVersionData);
    int delDBVersionData(DBVersionData dBVersionData);
    DBVersionData queDBVersionData(DBVersionData dBVersionData);
//    int altDBVersionData(DBVersionData dBVersionData);

    // 获取所有的记录
    QList<DBVersionData> queAllDBVersionData();

public:
    // 路径 静态变量
    static QString dbFilePath;
    static QString imgMoldFilePath;
    static QString imgNgFilePath;
    static QString txtNgFilePath;

private:
    // 数据检测
    bool checkShapeItemData(ShapeItemData itemData);
    bool checkImgMoldData(ImageMoldData imgData);
    bool checkNGRecordData(NGRecordData recordData);
    bool checkOptRecordData(OptRecordData recordData);
    bool checkCameraIPData(CameraIPData cameraIPData);

private:
    QSqlDatabase m_database;    //数据库
    QMutex m_mutex;             // 互斥锁
    bool m_databaseOpenFlag = false;    // 数据库打开标识
};

#endif // MYDATABASE_H
