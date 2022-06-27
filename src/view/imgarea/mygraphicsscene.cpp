#include <QDebug>

#include "mygraphicsscene.h"

MyGraphicsScene::MyGraphicsScene(QObject *parent)
    : QGraphicsScene(parent)
{
    m_isCreatePolygon = false;
    m_isCreateCurve = false;
    m_isPauseCurve  = false;
}

void MyGraphicsScene::startCreatePolygon()
{
    m_isCreatePolygon = true;
    m_ploygonList.clear();
}

void MyGraphicsScene::startCreateCurve()
{
    m_isCreateCurve = true;
    m_isPauseCurve  = true;
    m_curveList.clear();
}

void MyGraphicsScene::finishCreatePloygon()
{
    QPointF p = QPointF();
    emit updatePolyPoint(p, m_ploygonList, true);
    emit createPolyFinished();

    m_isCreatePolygon = false;
    m_ploygonList.clear();
}

void MyGraphicsScene::finishCreateCurve()
{
    QPointF p = QPointF();
    if (m_curveList.size() > 1) {
        emit updateCurvePoint(p, m_curveList, true);
        emit createCurveFinished();
    }
    m_isCreateCurve = false;
    m_isPauseCurve = false;
    m_curveList.clear();
}

void MyGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_isCreatePolygon) {
        QPointF p(event->scenePos().x(), event->scenePos().y());

        if (event->buttons() == Qt::LeftButton) {
            m_ploygonList.push_back(p);
            emit updatePolyPoint(p, m_ploygonList, false);
        } else if (event->buttons() == Qt::RightButton) {
            finishCreatePloygon();
        }
    } else if (m_isCreateCurve) {
        QPointF p(event->scenePos().x(), event->scenePos().y());

        if (event->buttons() == Qt::LeftButton) {
            m_isPauseCurve = false;
            m_curveList.push_back(p);

            emit updateCurvePoint(p, m_curveList, false);
        } else if (event->buttons() == Qt::RightButton) {
            finishCreateCurve();

        }
    } else {
        QGraphicsScene::mousePressEvent(event);
    }
}

void MyGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_isCreateCurve && !m_isPauseCurve) {
        QPointF p(event->scenePos().x(), event->scenePos().y());
        m_curveList.push_back(p);

        emit updateCurvePoint(p, m_curveList, false);
    } else {
        QGraphicsScene::mouseMoveEvent(event);
    }
}

void MyGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_isCreateCurve) {
        m_isPauseCurve = true;
    } else {
        QGraphicsScene::mouseReleaseEvent(event);
    }
}
