#include <QSqlQuery>
#include <QMessageBox>
#include <QApplication>
#include <QSqlError>
#include <QFileInfo>
#include <QSqlRecord>
#include <QDebug>

#include "mydatabase.h"

MyDataBase *MyDataBase::getInstance()
{
    static MyDataBase *instance = nullptr;
    if (nullptr == instance) {
        try {
            instance = new MyDataBase;
        } catch (const std::runtime_error &re) {
            qDebug() << "runtime_error:" << re.what();
        }
    }
    return instance;
}

MyDataBase::MyDataBase(QObject *parent)
    : QObject(parent)
{
    qDebug() << QSqlDatabase::drivers();    // 当前环境支持哪些数据库
    QMutexLocker lockData(&m_mutex);        // 加锁，函数执行完后自动解锁

    m_database=QSqlDatabase::addDatabase("QSQLITE");
    QString dirPath = QString("./data/ver1.0_");

    // TODO
//    QFileInfo oldVersion(dirPath + "mymusic.db");
//    if(oldVersion.exists())
//    {
//        读取旧版本数据库内容，并添加至新版本数据库函数
//    }

    // todo 数据库路径问题
//    m_database.setDatabaseName(dirPath + "monitor.db");
    m_database.setDatabaseName(QCoreApplication::applicationDirPath()+"//monitor.db");

    initDataBase();
}

MyDataBase::~MyDataBase()
{
    if (true == m_databaseOpenFlag) {
        m_database.close();
    }
}

int MyDataBase::initDataBase()
{
    if (!m_database.open()) {
        m_databaseOpenFlag = true;

        QMessageBox::warning(0, QObject::tr("Database Error"), m_database.lastError().text());
        return DB_UNCONNECT;
    }

    bool queryRes = true;
    QSqlQuery queryInit(m_database);
    // 新建表
    queryRes &= queryInit.exec(QString("create table if not exists shape_item ("
                                       "id INTEGER primary key AUTOINCREMENT,"
                                       "is_detect INTEGER,"
                                       "mold_id INTEGER,"
                                       "type INTEGER,"
                                       "center VARCHAR(20),"
                                       "edge VARCHAR(20),"
                                       "point_list VARCHAR(500),"
                                       "accuracy INTEGER,"
                                       "pixel INTEGER,"
                                       "other1 VARCHAR,"
                                       "other2 VARCHAR)"
                                       ));

    queryRes &= queryInit.exec(QString("create table if not exists ng_record ("
                                       "id INTEGER primary key AUTOINCREMENT,"
                                       "time VARCHAR,"
                                       "camera_id INTEGER,"
                                       "scene_id INTEGER,"
                                       "result BOOLEAN)"
                                       ));

    if (true == queryRes) {
        return 1;
    } else {
        return CREATE_TABLE_FAILED;
    }
}

int MyDataBase::addShapeItemData(ShapeItemData itemData)
{
    bool queryRes = true;

    if (!checkShapeItemData(itemData)) {
        return INVALID_INPUT;
    } else {
        if (m_database.isValid()) {
            QSqlQuery query;

            query.prepare("INSERT INTO shape_item (is_detect, mold_id, type, center, edge, point_list, accuracy, pixel) VALUES "
                          "(:is_detect, :mold_id, :type, :center, :edge, :point_list, :accuracy, :pixel)");

            query.bindValue(":is_detect",  QString::number(itemData.isDetect));
            query.bindValue(":mold_id",    QString::number(itemData.moldId));
            query.bindValue(":type",       QString::number(itemData.type));
            query.bindValue(":center",     itemData.center);
            query.bindValue(":edge",       itemData.edge);
            query.bindValue(":point_list", itemData.pointList);
            query.bindValue(":accuracy",   QString::number(itemData.accuracy));
            query.bindValue(":pixel",      QString::number(itemData.pixel));

            queryRes = query.exec();

            if (queryRes) {
                return DB_OP_SUCC;
            } else {
                return DB_OP_ADD_FAILED;
            }
        } else {
            return DB_UNCONNECT;
        }
    }
    return DB_OP_SUCC;
}

int MyDataBase::delShapeItemData(ShapeItemData itemData)
{
    bool queryRes = true;

    if (checkShapeItemData(itemData)) {
        QSqlQuery query;

        query.prepare("DELETE FROM shape_item WHERE is_detect=:is_detect and mold_id=:mold_id");

        query.bindValue(":is_detect", QString::number(itemData.isDetect));
        query.bindValue(":mold_id",   QString::number(itemData.moldId));

        queryRes = query.exec();
    }

    return DB_OP_SUCC;
}

ShapeItemData MyDataBase::queShapeItemData(ShapeItemData itemData)
{
    ShapeItemData resData;

    QSqlQuery query;
    bool queryRes = true;

    query.prepare("SELECT * FROM shape_item WHERE is_detect=:is_detect and mold_id=:mold_id");

    query.bindValue(":is_detect", QString::number(itemData.isDetect));
    query.bindValue(":mold_id",   QString::number(itemData.moldId));

    queryRes = query.exec();

    if (query.next()) {
        resData.isDetect = query.value("is_detect").toInt();
        resData.moldId   = query.value("mold_id").toInt();
        resData.type     = query.value("type").toInt();
        resData.center   = query.value("center").toString();
        resData.edge     = query.value("edge").toString();
        resData.pointList = query.value("point_list").toString();
        resData.accuracy  = query.value("accuracy").toInt();
        resData.pixel     = query.value("pixel").toInt();
    }

    return resData;
}

int MyDataBase::altShapeItemData(ShapeItemData itemData)
{
    bool queryRes = true;

    if (!checkShapeItemData(itemData)) {
        return INVALID_INPUT;
    } else {
        if (m_database.isValid()) {
            QSqlQuery query;

            query.prepare("UPDATE shape_item SET type=:type, center=:center, edge=:edge, point_list=:point_list, accuracy=:accuracy, pixel=:pixel"
                          "WHERE is_detect=:is_detect and mold_id=:mold_id");

            query.bindValue(":is_detect",  QString::number(itemData.isDetect));
            query.bindValue(":mold_id",    QString::number(itemData.moldId));
            query.bindValue(":type",       QString::number(itemData.type));
            query.bindValue(":center",     itemData.center);
            query.bindValue(":edge",       itemData.edge);
            query.bindValue(":point_list", itemData.pointList);
            query.bindValue(":accuracy",   QString::number(itemData.accuracy));
            query.bindValue(":pixel",      QString::number(itemData.pixel));

            queryRes = query.exec();

            if (queryRes) {
                return DB_OP_SUCC;
            } else {
                return DB_OP_ADD_FAILED;
            }
        } else {
            return DB_UNCONNECT;
        }
    }
    return DB_OP_SUCC;
}

int MyDataBase::addNGRecordData(NGRecordData recordData)
{
    bool queryRes = true;

    if (!checkNGRecordData(recordData)) {
        return INVALID_INPUT;
    } else {
        if (m_database.isValid()) {
            QSqlQuery query;

            query.prepare("INSERT INTO ng_record (time, camera_id, scene_id, result) VALUES "
                          "(:time, :camera_id, :scene_id, :result)");

            query.bindValue(":time",      recordData.time);
            query.bindValue(":camera_id", QString::number(recordData.cameraId));
            query.bindValue(":scene_id",  QString::number(recordData.sceneId));
            query.bindValue(":result",    recordData.result);

            queryRes = query.exec();

            if (queryRes) {
                return DB_OP_SUCC;
            } else {
                return DB_OP_ADD_FAILED;
            }
        } else {
            return DB_UNCONNECT;
        }
    }
    return DB_OP_SUCC;
}

int MyDataBase::delNGRecordData(NGRecordData recordData)
{
    bool queryRes = true;

    if (checkNGRecordData(recordData)) {
        QSqlQuery query;

        query.prepare("DELETE FROM ng_record WHERE camera_id=:camera_id and scene_id=:scene_id");

        query.bindValue(":camera_id", QString::number(recordData.cameraId));
        query.bindValue(":scene_id",  QString::number(recordData.sceneId));

        queryRes = query.exec();
    }

    return DB_OP_SUCC;
}

NGRecordData MyDataBase::queNGRecordData(NGRecordData recordData)
{
    NGRecordData resData;

    QSqlQuery query;
    bool queryRes = true;

    query.prepare("SELECT * FROM ng_record WHERE camera_id=:camera_id and scene_id=:scene_id");

    query.bindValue(":camera_id", QString::number(recordData.cameraId));
    query.bindValue(":scene_id",  QString::number(recordData.sceneId));

    queryRes = query.exec();

    if (query.next()) {
        resData.time     = query.value("time").toString();
        resData.cameraId = query.value("camera_id").toInt();
        resData.sceneId  = query.value("scene_id").toInt();
        resData.result   = query.value("result").toString();
    }

    return resData;
}

int MyDataBase::altNGRecordData(NGRecordData recordData)
{
    bool queryRes = true;

    if (!checkNGRecordData(recordData)) {
        return INVALID_INPUT;
    } else {
        if (m_database.isValid()) {
            QSqlQuery query;

            query.prepare("UPDATE ng_record SET time=:time, result=:result"
                          "WHERE camera_id=:camera_id and scene_id=:scene_id");

            query.bindValue(":time",      recordData.time);
            query.bindValue(":camera_id", QString::number(recordData.cameraId));
            query.bindValue(":scene_id",  QString::number(recordData.sceneId));
            query.bindValue(":result",    recordData.result);

            queryRes = query.exec();

            if (queryRes) {
                return DB_OP_SUCC;
            } else {
                return DB_OP_ADD_FAILED;
            }
        } else {
            return DB_UNCONNECT;
        }
    }
    return DB_OP_SUCC;
}

bool MyDataBase::checkShapeItemData(ShapeItemData itemData)
{
    bool checkRes = true;

    if (itemData.isDetect != 0 && itemData.isDetect != 1) {
        return false;
    }

    if (itemData.moldId < 0) {
        return false;
    }

    return checkRes;
}

bool MyDataBase::checkNGRecordData(NGRecordData recordData)
{
    bool checkRes = true;

    if (recordData.cameraId < 0) {
        return false;
    }

    if (recordData.sceneId < 0) {
        return false;
    }

    if (recordData.result.isEmpty()) {
        return false;
    }

    return checkRes;
}
