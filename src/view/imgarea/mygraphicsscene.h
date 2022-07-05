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

    void setData();

    void startCreatePolygon();
    void finishCreatePloygon();

    void startCreateCurve();
    void finishCreateCurve();

    void startCreateRect();
    void finishCreateRect();

    void startCreateCircle();
    void finishCreateCircle();

    void startCreateConCircle();
    void finishCreateConCircle();


protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

signals:
    void updatePolyPoint(QPointF p, QList<QPointF> list, bool isCenter);
    void createPolyFinished();

    void updateCurvePoint(QPointF p, QList<QPointF> list, bool isCenter);
    void createCurveFinished();

protected:
    QList<QPointF> m_ploygonList;
    QList<QPointF> m_curveList;

    bool m_isCreatePolygon;
    bool m_isCreateCurve;
    bool m_isPauseCurve;

    bool m_isCreateRect;
    bool m_isCreatingRect;

    bool m_isCreateCircle;
    bool m_isCreatingCircle;

    bool m_isCreateConCir;
    bool m_isCreatingConCir;

    bool m_isAddPolygonPoint;

//    MyRectangle *rectangle;
};

#endif // MYGRAPHICSSCENE_H
