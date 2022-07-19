#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>
#include <QFileInfo>
#include <QSqlRecord>
#include <QDebug>

#include "mydatabase.h"

QString MyDataBase::dbFilePath = "";
QString MyDataBase::imgFilePath = "";
QString MyDataBase::imgNgFilePath = "";

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
//    qDebug() << QSqlDatabase::drivers();    // 当前环境支持哪些数据库
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
    // create folder
    dbFilePath  = QCoreApplication::applicationDirPath() + "/data";
    imgFilePath = dbFilePath + "/imgmold";
    imgNgFilePath = dbFilePath + "/imgng";

    QDir dir(dbFilePath);
    if (!dir.exists()) {
        bool ismkdir = dir.mkdir(dbFilePath);
        ismkdir = dir.mkdir(imgFilePath);
        ismkdir = dir.mkdir(imgNgFilePath);
        if(!ismkdir)
            qDebug() << "Create path fail" << endl;
        else
            qDebug() << "Create fullpath success" << endl;
    }
//    m_database.setDatabaseName(dirPath + "monitor.db");
    m_database.setDatabaseName(dbFilePath + "/monitor.db");
    qDebug() << QDir::homePath();

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
                                       "camera_id INTEGER,"
                                       "scene_id INTEGER,"
                                       "mold_id INTEGER,"
                                       "item_id INTEGER,"
                                       "type INTEGER,"
                                       "center VARCHAR(20),"
                                       "edge VARCHAR(20),"
                                       "point_list TEXT,"
                                       "accuracy INTEGER,"
                                       "pixel INTEGER,"
                                       "other1 VARCHAR,"
                                       "other2 VARCHAR)"
                                       ));

    queryRes &= queryInit.exec(QString("create table if not exists image_mold ("
                                       "id INTEGER primary key AUTOINCREMENT,"
                                       "camera_id INTEGER,"
                                       "scene_id INTEGER,"
                                       "mold_id INTEGER,"
                                       "img_path VARCHAR,"
                                       "img_content VARCHAR,"
                                       "time VARCHAR,"
                                       "other1 VARCHAR,"
                                       "other2 VARCHAR)"
                                       ));

    queryRes &= queryInit.exec(QString("create table if not exists ng_record ("
                                       "id INTEGER primary key AUTOINCREMENT,"
                                       "time VARCHAR,"
                                       "camera_id INTEGER,"
                                       "scene_id INTEGER,"
                                       "result VARCHAR,"
                                       "img_path VARCHAR,"
                                       "other1 VARCHAR,"
                                       "other2 VARCHAR)"
                                       ));

    queryRes &= queryInit.exec(QString("create table if not exists opt_record ("
                                       "id INTEGER primary key AUTOINCREMENT,"
                                       "time VARCHAR,"
                                       "optor_name VARCHAR,"
                                       "opt_log VARCHAR,"
                                       "other1 VARCHAR,"
                                       "other2 VARCHAR)"
                                       ));

    queryRes &= queryInit.exec(QString("create table if not exists camera_ip_deploy ("
                                       "id INTEGER primary key AUTOINCREMENT,"
                                       "camera_id INTEGER,"
                                       "serial_id VARCHAR,"
                                       "nick_name VARCHAR,"
                                       "port_ip VARCHAR,"
                                       "state VARCHAR,"
                                       "camera_ip VARCHAR,"
                                       "other1 VARCHAR,"
                                       "other2 VARCHAR)"
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

            query.prepare("INSERT INTO shape_item (camera_id, scene_id, mold_id, type, center, edge, point_list, accuracy, pixel) VALUES "
                          "(:camera_id, :scene_id, :mold_id, :type, :center, :edge, :point_list, :accuracy, :pixel)");

            query.bindValue(":camera_id",  QString::number(itemData.cameraId));
            query.bindValue(":scene_id",   QString::number(itemData.sceneId));
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

        query.prepare("DELETE FROM shape_item WHERE camera_id=:camera_id and scene_id=:scene_id and mold_id=:mold_id");

        query.bindValue(":camera_id", QString::number(itemData.cameraId));
        query.bindValue(":scene_id",  QString::number(itemData.sceneId));
        query.bindValue(":mold_id",   QString::number(itemData.moldId));

        queryRes = query.exec();
    }

    return DB_OP_SUCC;
}

int MyDataBase::delSceneShapeItemData(ShapeItemData itemData)
{
    bool queryRes = true;

    if (checkShapeItemData(itemData)) {
        QSqlQuery query;

        query.prepare("DELETE FROM shape_item WHERE camera_id=:camera_id and scene_id=:scene_id");

        query.bindValue(":camera_id", QString::number(itemData.cameraId));
        query.bindValue(":scene_id",  QString::number(itemData.sceneId));

        queryRes = query.exec();
    }

    return DB_OP_SUCC;
}

QList<ShapeItemData> MyDataBase::queShapeItemData(ShapeItemData itemData)
{
    QList<ShapeItemData> resDataList;

    QSqlQuery query;
    bool queryRes = true;

    query.prepare("SELECT * FROM shape_item WHERE camera_id=:camera_id and scene_id=:scene_id");

    query.bindValue(":camera_id", QString::number(itemData.cameraId));
    query.bindValue(":scene_id",  QString::number(itemData.sceneId));

    queryRes = query.exec();

    while (query.next()) {
        ShapeItemData resData;

        resData.cameraId  = query.value("camera_id").toInt();
        resData.sceneId   = query.value("scene_id").toInt();
        resData.moldId    = query.value("mold_id").toInt();
        resData.type      = query.value("type").toInt();
        resData.center    = query.value("center").toString();
        resData.edge      = query.value("edge").toString();
        resData.pointList = query.value("point_list").toString();
        resData.accuracy  = query.value("accuracy").toInt();
        resData.pixel     = query.value("pixel").toInt();

        resDataList.append(resData);
    }

    return resDataList;
}

int MyDataBase::altShapeItemData(ShapeItemData itemData)
{
    bool queryRes = true;

    if (!checkShapeItemData(itemData)) {
        return INVALID_INPUT;
    } else {
        if (m_database.isValid()) {
            QSqlQuery query;

            query.prepare("UPDATE shape_item SET type=:type, center=:center, edge=:edge, point_list=:point_list, accuracy=:accuracy, pixel=:pixel "
                          "WHERE camera_id=:camera_id and scene_id=:scene_id and mold_id=:mold_id");

            query.bindValue(":camera_id",  QString::number(itemData.cameraId));
            query.bindValue(":scene_id",  QString::number(itemData.sceneId));
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

int MyDataBase::getMoldNum(ShapeItemData itemData)
{
    int count = 0;

    QSqlQuery query;
    bool queryRes = true;

    query.prepare("SELECT COUNT(DISTINCT mold_id) FROM shape_item WHERE camera_id=:camera_id and scene_id=:scene_id");

    query.bindValue(":camera_id", QString::number(itemData.cameraId));
    query.bindValue(":scene_id",  QString::number(itemData.sceneId));

    queryRes = query.exec();
    query.next();

    count = query.value(0).toInt();
    return count;
}

int MyDataBase::updateItemMoldId(ShapeItemData itemData)
{
    bool queryRes = true;

    if (!checkShapeItemData(itemData)) {
        return INVALID_INPUT;
    } else {
        if (m_database.isValid()) {
            QSqlQuery query;

            query.prepare("UPDATE shape_item SET mold_id=mold_id-1 "
                          "WHERE camera_id=:camera_id and scene_id=:scene_id and mold_id>:mold_id");

            query.bindValue(":camera_id", QString::number(itemData.cameraId));
            query.bindValue(":scene_id",  QString::number(itemData.sceneId));
            query.bindValue(":mold_id",   QString::number(itemData.moldId));

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

int MyDataBase::addImgMoldData(ImageMoldData imgData)
{
    bool queryRes = true;

    if (!checkImgMoldData(imgData)) {
        return INVALID_INPUT;
    } else {
        if (m_database.isValid()) {
            QSqlQuery query;

            query.prepare("INSERT INTO image_mold (camera_id, scene_id, mold_id, img_path, img_content, time) VALUES "
                          "(:camera_id, :scene_id, :mold_id, :img_path, :img_content, :time)");

            query.bindValue(":camera_id",   QString::number(imgData.cameraId));
            query.bindValue(":scene_id",    QString::number(imgData.sceneId));
            query.bindValue(":mold_id",     QString::number(imgData.moldId));
            query.bindValue(":img_path",    imgData.imgPath);
            query.bindValue(":img_content", imgData.imgContent);
            query.bindValue(":time",        imgData.time);

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

int MyDataBase::delImgMoldData(ImageMoldData imgData)
{
    bool queryRes = true;

    if (checkImgMoldData(imgData)) {
        QSqlQuery query;

        query.prepare("DELETE FROM image_mold WHERE camera_id=:camera_id and scene_id=:scene_id and mold_id=:mold_id");

        query.bindValue(":camera_id", QString::number(imgData.cameraId));
        query.bindValue(":scene_id",  QString::number(imgData.sceneId));
        query.bindValue(":mold_id",   QString::number(imgData.moldId));

        queryRes = query.exec();
    }

    return DB_OP_SUCC;
}

ImageMoldData MyDataBase::queImgMoldData(ImageMoldData imgData)
{
    ImageMoldData resData;

    QSqlQuery query;
    bool queryRes = true;

    query.prepare("SELECT * FROM image_mold WHERE camera_id=:camera_id and scene_id=:scene_id and mold_id=:mold_id");

    query.bindValue(":camera_id", QString::number(imgData.cameraId));
    query.bindValue(":scene_id",  QString::number(imgData.sceneId));
    query.bindValue(":mold_id",   QString::number(imgData.moldId));

    queryRes = query.exec();

    if (query.next()) {
        resData.cameraId   = query.value("camera_id").toInt();
        resData.sceneId    = query.value("scene_id").toInt();
        resData.moldId     = query.value("mold_id").toInt();
        resData.imgPath    = query.value("img_path").toString();
        resData.imgContent = query.value("img_content").toString();
        resData.time       = query.value("time").toString();
    }

    return resData;
}

int MyDataBase::altImgMoldData(ImageMoldData imgData)
{
    bool queryRes = true;

    if (!checkImgMoldData(imgData)) {
        return INVALID_INPUT;
    } else {
        if (m_database.isValid()) {
            QSqlQuery query;

            query.prepare("UPDATE image_mold SET img_path=:img_path, img_content=:img_content, time=:time "
                          "WHERE camera_id=:camera_id and scene_id=:scene_id and mold_id=:mold_id");

            query.bindValue(":camera_id",   QString::number(imgData.cameraId));
            query.bindValue(":scene_id",    QString::number(imgData.sceneId));
            query.bindValue(":mold_id",     QString::number(imgData.moldId));
            query.bindValue(":img_path",    imgData.imgPath);
            query.bindValue(":img_content", imgData.imgContent);
            query.bindValue(":time",        imgData.time);

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

QList<ImageMoldData> MyDataBase::queAllImgMoldData(ImageMoldData imgData)
{
    QList<ImageMoldData> resDataList;

    QSqlQuery query;
    bool queryRes = true;

    query.prepare("SELECT * FROM image_mold WHERE camera_id=:camera_id and scene_id=:scene_id");

    query.bindValue(":camera_id", QString::number(imgData.cameraId));
    query.bindValue(":scene_id",  QString::number(imgData.sceneId));

    queryRes = query.exec();

    while (query.next()) {
        ImageMoldData resData;

        resData.cameraId  = query.value("camera_id").toInt();
        resData.sceneId   = query.value("scene_id").toInt();
        resData.moldId    = query.value("mold_id").toInt();
        resData.imgPath   = query.value("img_path").toString();
        resData.imgContent= query.value("img_content").toString();
        resData.time      = query.value("time").toString();

        resDataList.append(resData);
    }

    return resDataList;
}

int MyDataBase::updateImgMoldId(ImageMoldData imgData)
{
    bool queryRes = true;

    if (!checkImgMoldData(imgData)) {
        return INVALID_INPUT;
    } else {
        if (m_database.isValid()) {
            QSqlQuery query;

            query.prepare("UPDATE image_mold SET mold_id=mold_id-1 "
                          "WHERE camera_id=:camera_id and scene_id=:scene_id and mold_id>:mold_id");

            query.bindValue(":camera_id", QString::number(imgData.cameraId));
            query.bindValue(":scene_id",  QString::number(imgData.sceneId));
            query.bindValue(":mold_id",   QString::number(imgData.moldId));

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

int MyDataBase::delSceneImgMoldData(ImageMoldData imgData)
{
    bool queryRes = true;

    if (checkImgMoldData(imgData)) {
        QSqlQuery query;

        query.prepare("DELETE FROM image_mold WHERE camera_id=:camera_id and scene_id=:scene_id");

        query.bindValue(":camera_id", QString::number(imgData.cameraId));
        query.bindValue(":scene_id",  QString::number(imgData.sceneId));

        queryRes = query.exec();
    }

    return DB_OP_SUCC;
}

int MyDataBase::getImageMoldNum(ImageMoldData imgData)
{
    int count = 0;

    QSqlQuery query;
    bool queryRes = true;

    query.prepare("SELECT COUNT(DISTINCT mold_id) FROM image_mold WHERE camera_id=:camera_id and scene_id=:scene_id");

    query.bindValue(":camera_id", QString::number(imgData.cameraId));
    query.bindValue(":scene_id",  QString::number(imgData.sceneId));

    queryRes = query.exec();
    query.next();

    count = query.value(0).toInt();
    return count;
}

int MyDataBase::addNGRecordData(NGRecordData recordData)
{
    bool queryRes = true;

    if (!checkNGRecordData(recordData)) {
        return INVALID_INPUT;
    } else {
        if (m_database.isValid()) {
            QSqlQuery query;

            query.prepare("INSERT INTO ng_record (time, camera_id, scene_id, result, img_path) VALUES "
                          "(:time, :camera_id, :scene_id, :result, :img_path)");

            query.bindValue(":time",      recordData.time);
            query.bindValue(":camera_id", QString::number(recordData.cameraId));
            query.bindValue(":scene_id",  QString::number(recordData.sceneId));
            query.bindValue(":result",    recordData.result);
            query.bindValue(":img_path",  recordData.imgPath);

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

    query.prepare("SELECT * FROM ng_record WHERE camera_id=:camera_id and scene_id=:scene_id and time=:time");

    query.bindValue(":camera_id", QString::number(recordData.cameraId));
    query.bindValue(":scene_id",  QString::number(recordData.sceneId));
    query.bindValue(":time",      recordData.time);

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

QList<NGRecordData> MyDataBase::queAllNGRecordData()
{
    QList<NGRecordData> resDataList;

    QSqlQuery query;
    bool queryRes = true;

    query.prepare("SELECT * FROM ng_record");

    queryRes = query.exec();

    while (query.next()) {
        NGRecordData resData;

        resData.cameraId  = query.value("camera_id").toInt();
        resData.sceneId   = query.value("scene_id").toInt();
        resData.time      = query.value("time").toString();
        resData.result    = query.value("result").toString();
        resData.imgPath   = query.value("img_path").toString();

        resDataList.append(resData);
    }

    return resDataList;
}

int MyDataBase::addOptRecordData(OptRecordData recordData)
{
    bool queryRes = true;

    if (!checkOptRecordData(recordData)) {
        return INVALID_INPUT;
    } else {
        if (m_database.isValid()) {
            QSqlQuery query;

            query.prepare("INSERT INTO opt_record (time, optor_name, opt_log) VALUES "
                          "(:time, :optor_name, :opt_log)");

            query.bindValue(":time",       recordData.time);
            query.bindValue(":optor_name", recordData.optorName);
            query.bindValue(":opt_log",    recordData.optLog);

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

int MyDataBase::delOptRecordData(OptRecordData recordData)
{
    bool queryRes = true;

    if (checkOptRecordData(recordData)) {
        QSqlQuery query;

        query.prepare("DELETE FROM opt_record WHERE id=:id");

        query.bindValue(":id", QString::number(recordData.recordId));

        queryRes = query.exec();
    }

    return DB_OP_SUCC;
}

OptRecordData MyDataBase::queOptRecordData(OptRecordData recordData)
{
    OptRecordData resData;

    QSqlQuery query;
    bool queryRes = true;

    query.prepare("SELECT * FROM opt_record WHERE id=:id");

    query.bindValue(":id", QString::number(recordData.recordId));

    queryRes = query.exec();

    if (query.next()) {
        resData.recordId   = query.value("id").toInt();
        resData.time       = query.value("time").toString();
        resData.optorName  = query.value("optor_name").toInt();
        resData.optLog     = query.value("opt_log").toString();
    }

    return resData;
}

//int MyDataBase::altOptRecordData(OptRecordData recordData)
//{

//}

QList<OptRecordData> MyDataBase::queAllOptRecordData()
{
    QList<OptRecordData> resDataList;

    QSqlQuery query;
    bool queryRes = true;

    query.prepare("SELECT * FROM opt_record");

    queryRes = query.exec();

    while (query.next()) {
        OptRecordData resData;

        resData.recordId  = query.value("id").toInt();
        resData.time      = query.value("time").toString();
        resData.optorName = query.value("optor_name").toString();
        resData.optLog    = query.value("opt_log").toString();

        resDataList.append(resData);
    }

    return resDataList;
}

int MyDataBase::addCameraIPData(CameraIPData cameraIPData)
{
    bool queryRes = true;

    if (!checkCameraIPData(cameraIPData)) {
        return INVALID_INPUT;
    } else {
        if (m_database.isValid()) {
            QSqlQuery query;

            query.prepare("INSERT INTO camera_ip_deploy (camera_id, serial_id, nick_name, port_ip, state, camera_ip) VALUES "
                          "(:camera_id, :serial_id, :nick_name, :port_ip, :state, :camera_ip)");

            query.bindValue(":camera_id", QString::number(cameraIPData.cameraId));
            query.bindValue(":serial_id", cameraIPData.serialId);
            query.bindValue(":nick_name", cameraIPData.nickName);
            query.bindValue(":port_ip",   cameraIPData.portIp);
            query.bindValue(":state",     cameraIPData.state);
            query.bindValue(":camera_ip", cameraIPData.cameraIp);

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

int MyDataBase::delCameraIPData(CameraIPData cameraIPData)
{
    bool queryRes = true;

    if (checkCameraIPData(cameraIPData)) {
        QSqlQuery query;

        query.prepare("DELETE FROM camera_ip_deploy WHERE camera_id=:camera_id");

        query.bindValue(":camera_id", QString::number(cameraIPData.cameraId));

        queryRes = query.exec();
    }

    return DB_OP_SUCC;
}

CameraIPData MyDataBase::queCameraIPData(CameraIPData cameraIPData)
{
    CameraIPData resData;
    resData.cameraId = -1;

    QSqlQuery query;
    bool queryRes = true;

    query.prepare("SELECT * FROM camera_ip_deploy WHERE serial_id=:serial_id");

    query.bindValue(":serial_id", cameraIPData.serialId);

    queryRes = query.exec();

    if (query.next()) {
        resData.cameraId = query.value("camera_id").toInt();
        resData.serialId = query.value("serial_id").toString();
        resData.nickName = query.value("nick_name").toString();
        resData.portIp   = query.value("port_ip").toString();
        resData.state    = query.value("state").toString();
        resData.cameraIp = query.value("camera_ip").toString();
    }

    return resData;
}

int MyDataBase::altCameraIPData(CameraIPData cameraIPData)
{
    bool queryRes = true;

    if (!checkCameraIPData(cameraIPData)) {
        return INVALID_INPUT;
    } else {
        if (m_database.isValid()) {
            QSqlQuery query;

            query.prepare("UPDATE camera_ip_deploy SET serial_id=:serial_id, nick_name=:nick_name, port_ip=:port_ip, state=:state, camera_ip=:camera_ip"
                          "WHERE camera_id=:camera_id");

            query.bindValue(":serial_id", cameraIPData.serialId);
            query.bindValue(":nick_name", cameraIPData.nickName);
            query.bindValue(":port_ip",   cameraIPData.portIp);
            query.bindValue(":state",     cameraIPData.state);
            query.bindValue(":camera_ip", cameraIPData.cameraIp);

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

QList<CameraIPData> MyDataBase::queAllCameraIPData()
{
    QList<CameraIPData> resDataList;

    QSqlQuery query;
    bool queryRes = true;

    query.prepare("SELECT * FROM camera_ip_deploy");

    queryRes = query.exec();

    while (query.next()) {
        CameraIPData resData;

        resData.cameraId = query.value("camera_id").toInt();
        resData.serialId = query.value("serial_id").toString();
        resData.nickName = query.value("nick_name").toString();
        resData.portIp   = query.value("port_ip").toString();
        resData.state    = query.value("state").toString();
        resData.cameraIp = query.value("camera_ip").toString();

        resDataList.append(resData);
    }

    return resDataList;
}

bool MyDataBase::checkShapeItemData(ShapeItemData itemData)
{
    bool checkRes = true;

    if (itemData.sceneId != 1 && itemData.sceneId != 2) {
        return false;
    }

    if (itemData.moldId < 0) {
        return false;
    }

    return checkRes;
}

bool MyDataBase::checkImgMoldData(ImageMoldData imgData)
{
    bool checkRes = true;

    if (imgData.sceneId != 1 && imgData.sceneId != 2) {
        return false;
    }

    if (imgData.moldId < 0) {
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

bool MyDataBase::checkOptRecordData(OptRecordData recordData)
{
    bool checkRes = true;

    if (recordData.optLog.isEmpty()) {
        return false;
    }

    return checkRes;
}

bool MyDataBase::checkCameraIPData(CameraIPData cameraIPData)
{
    bool checkRes = true;

    if (cameraIPData.cameraId < 0) {
        return false;
    }

    if (cameraIPData.serialId.isEmpty()) {
        return false;
    }

    return checkRes;
}
