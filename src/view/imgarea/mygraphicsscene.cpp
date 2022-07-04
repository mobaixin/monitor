#include <QDebug>

#include "mygraphicsscene.h"
#include "src/view/bottombar/bottombar.h"

MyGraphicsScene::MyGraphicsScene(QObject *parent)
    : QGraphicsScene(parent)
{
    m_isCreatePolygon = false;

    m_isCreateCurve = false;
    m_isPauseCurve  = false;

    m_isCreateRect   = false;
    m_isCreatingRect = false;

    m_isCreateCircle   = false;
    m_isCreatingCircle = false;
}

void MyGraphicsScene::startCreatePolygon()
{
    m_isCreatePolygon = true;
    m_ploygonList.clear();
}

void MyGraphicsScene::finishCreatePloygon()
{
    QPointF p = QPointF();
    emit updatePolyPoint(p, m_ploygonList, true);
    emit createPolyFinished();

    m_isCreatePolygon = false;
    m_ploygonList.clear();
}

void MyGraphicsScene::startCreateCurve()
{
    m_isCreateCurve = true;
    m_isPauseCurve  = true;
    m_curveList.clear();
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

void MyGraphicsScene::startCreateRect()
{
    m_isCreateRect = true;
}

void MyGraphicsScene::finishCreateRect()
{
    m_isCreateRect   = false;
    m_isCreatingRect = false;
}

void MyGraphicsScene::startCreateCircle()
{
    m_isCreateCircle = true;
}

void MyGraphicsScene::finishCreateCircle()
{
    m_isCreateCircle   = false;
    m_isCreatingCircle = false;
}

void MyGraphicsScene::addMyItem(QGraphicsItem *item)
{
    this->addItem(item);
    this->addSimpleText("123");
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
    } else if (m_isCreateRect) {
        QPointF p(event->scenePos().x(), event->scenePos().y());

        BottomBar::getInstance()->createRect(p);
        m_isCreatingRect = true;
    } else if (m_isCreateCircle) {
        QPointF p(event->scenePos().x(), event->scenePos().y());

        BottomBar::getInstance()->createCircle(p);
        m_isCreatingCircle = true;
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
    } else if (m_isCreateRect && m_isCreatingRect) {
        QPointF p(event->scenePos().x(), event->scenePos().y());
        MyRectangle *myRect = BottomBar::getInstance()->getNewMyRect();
        MyPointItem *edgeItem = myRect->getEdgeItem();

        edgeItem->setPoint(event->scenePos());
        edgeItem->setPos(edgeItem->getPoint());

        myRect->setEdge(p);
        edgeItem->scene()->update();
    } else if (m_isCreateCircle && m_isCreatingCircle) {
        QPointF p(event->scenePos().x(), event->scenePos().y());
        MyCircle *myCircle = BottomBar::getInstance()->getNewMyCircle();
        MyPointItem *edgeItem = myCircle->getEdgeItem();

        edgeItem->setPoint(event->scenePos());
        edgeItem->setPos(edgeItem->getPoint());

        myCircle->setEdge(p);
        edgeItem->scene()->update();
        myCircle->updateRadius();
    }
    else {
        QGraphicsScene::mouseMoveEvent(event);
    }
}

void MyGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_isCreateCurve) {
        m_isPauseCurve = true;
    } else if (m_isCreatingRect) {
        finishCreateRect();
    } else if (m_isCreatingCircle) {
        finishCreateCircle();
    }

    QGraphicsScene::mouseReleaseEvent(event);
}
