#ifndef MYGRAPHICSSCENE_H
#define MYGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QObject>
#include "mypointitem.h"

class MyGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit MyGraphicsScene(QObject *parent = nullptr);

    // 初始化数据
    void setData();

    // 多边形创建和完成
    void startCreatePolygon();
    void finishCreatePloygon();

    // 曲线创建和完成
    void startCreateCurve();
    void finishCreateCurve();

    // 矩形创建和完成
    void startCreateRect();
    void finishCreateRect();

    // 圆形创建和完成
    void startCreateCircle();
    void finishCreateCircle();

    // 环形创建和完成
    void startCreateConCircle();
    void finishCreateConCircle();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

signals:
    // 多边形 新增点和完成信号
    void updatePolyPoint(QPointF p, QList<QPointF> list, bool isCenter);
    void createPolyFinished();

    // 曲线 新增点和完成信号
    void updateCurvePoint(QPointF p, QList<QPointF> list, bool isCenter);
    void createCurveFinished();

protected:
    QList<QPointF> m_ploygonList;   // 多边形边缘点列表
    QList<QPointF> m_curveList;     // 曲线边缘点列表

    bool m_isCreatePolygon = false; // 创建多边形
    bool m_isCreateCurve   = false; // 创建曲线
    bool m_isPauseCurve    = false; // 暂停创建曲线

    bool m_isCreateRect;            // 创建矩形
    bool m_isCreatingRect;          // 正在创建矩形

    bool m_isCreateCircle;          // 创建圆形
    bool m_isCreatingCircle;        // 正在创建圆形

    bool m_isCreateConCir;          // 创建环形
    bool m_isCreatingConCir;        // 正在创建环形

    bool m_isAddPolygonPoint;       // 判断添加多边形边缘点

//    MyRectangle *rectangle;
};

#endif // MYGRAPHICSSCENE_H
