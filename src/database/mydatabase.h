#ifndef MYDATABASE_H
#define MYDATABASE_H

#include <QSqlDatabase>
#include <QObject>
#include <QFile>
#include <QList>
#include <QString>
#include <QThread>
#include <QMutexLocker>

const QString SHAPEITEM = "shape_item"; //图形标注表
const QString NGRECORD  = "ng_record";  //NG记录表

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

typedef struct ShapeItemData{
    int cameraId;
    int sceneId;
    int moldId;
    int type;
    QString center;
    QString edge;
    QString pointList;
    int accuracy;
    int pixel;
}ShapeItemData;

typedef struct NGRecordData{
    QString time;
    int cameraId;
    int sceneId;
    QString result;
}NGRecordData;

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

    // ShapeItem表操作
    int addShapeItemData(ShapeItemData itemData);
    int delShapeItemData(ShapeItemData itemData);
    int delSceneShapeItemData(ShapeItemData itemData);
    QList<ShapeItemData> queShapeItemData(ShapeItemData itemData);
    int altShapeItemData(ShapeItemData itemData);

    int getMoldNum(ShapeItemData itemData);
    int updateItemMoldId(ShapeItemData itemData);

    // NGRecord表操作
    int addNGRecordData(NGRecordData recordData);
    int delNGRecordData(NGRecordData recordData);
    NGRecordData queNGRecordData(NGRecordData recordData);
    int altNGRecordData(NGRecordData recordData);

private:
    bool checkShapeItemData(ShapeItemData itemData);
    bool checkNGRecordData(NGRecordData recordData);

private:
    QSqlDatabase m_database;//数据库
    QMutex m_mutex;
    bool m_databaseOpenFlag = false;
};

#endif // MYDATABASE_H
