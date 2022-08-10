#include <QDebug>

#include "mygraphicsscene.h"
#include "src/view/bottombar/bottombar.h"

MyGraphicsScene::MyGraphicsScene(QObject *parent)
    : QGraphicsScene(parent)
{
    // 初始化数据
    setData();
}

// 初始化数据
void MyGraphicsScene::setData()
{
//    m_ploygonList = [];
//    m_curveList = [];

    if (m_isCreatePolygon) {
        BottomBar::getInstance()->updateCreatePolygon();
    }

    if (m_isCreateCurve) {
        BottomBar::getInstance()->updateCreateCurve();
    }

    m_isCreatePolygon = false;

    m_isCreateCurve = false;
    m_isPauseCurve  = false;

    m_isCreateRect   = false;
    m_isCreatingRect = false;

    m_isCreateCircle   = false;
    m_isCreatingCircle = false;

    m_isCreateConCir   = false;
    m_isCreatingConCir = false;
}

void MyGraphicsScene::startCreatePolygon()
{
    setData();
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
    setData();
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
    setData();
    m_isCreateRect = true;
}

void MyGraphicsScene::finishCreateRect()
{
    m_isCreateRect   = false;
    m_isCreatingRect = false;
}

void MyGraphicsScene::startCreateCircle()
{
    setData();
    m_isCreateCircle = true;
}

void MyGraphicsScene::finishCreateCircle()
{
    m_isCreateCircle   = false;
    m_isCreatingCircle = false;
}

void MyGraphicsScene::startCreateConCircle()
{
    setData();
    m_isCreateConCir = true;
}

void MyGraphicsScene::finishCreateConCircle()
{
    m_isCreateConCir   = false;
    m_isCreatingConCir = false;
}

void MyGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // 创建多边形
    if (m_isCreatePolygon) {
        QPointF p(event->scenePos().x(), event->scenePos().y());

        if (event->buttons() == Qt::LeftButton) {
            m_ploygonList.push_back(p);
            emit updatePolyPoint(p, m_ploygonList, false);
        } else if (event->buttons() == Qt::RightButton) {
//            finishCreatePloygon();
        }

    // 创建曲线
    } else if (m_isCreateCurve) {
        QPointF p(event->scenePos().x(), event->scenePos().y());

        if (event->buttons() == Qt::LeftButton) {
            m_isPauseCurve = false;
            m_curveList.push_back(p);

            emit updateCurvePoint(p, m_curveList, false);
        } else if (event->buttons() == Qt::RightButton) {
//            finishCreateCurve();

        }

    // 创建矩形
    } else if (m_isCreateRect) {
        QPointF p(event->scenePos().x(), event->scenePos().y());

        BottomBar::getInstance()->createRect(p);
        m_isCreatingRect = true;

    // 创建圆形
    } else if (m_isCreateCircle) {
        QPointF p(event->scenePos().x(), event->scenePos().y());

        BottomBar::getInstance()->createCircle(p);
        m_isCreatingCircle = true;

    // 创建环形
    } else if (m_isCreateConCir) {
        QPointF p(event->scenePos().x(), event->scenePos().y());

        BottomBar::getInstance()->createConCircle(p);
        m_isCreatingConCir = true;
    } else {
        QGraphicsScene::mousePressEvent(event);
    }
}

void MyGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // 曲线
    if (m_isCreateCurve && !m_isPauseCurve) {
        QPointF p(event->scenePos().x(), event->scenePos().y());
        m_curveList.push_back(p);

        emit updateCurvePoint(p, m_curveList, false);

    // 矩形
    } else if (m_isCreateRect && m_isCreatingRect) {
        QPointF p(event->scenePos().x(), event->scenePos().y());
        MyRectangle *myRect = BottomBar::getInstance()->getNewMyRect();
        MyPointItem *edgeItem = myRect->getEdgeItem();

        edgeItem->setPoint(event->scenePos());
        edgeItem->setPos(edgeItem->getPoint());

        myRect->setEdge(p);
        edgeItem->scene()->update();

    // 圆形
    } else if (m_isCreateCircle && m_isCreatingCircle) {
        QPointF p(event->scenePos().x(), event->scenePos().y());
        MyCircle *myCircle = BottomBar::getInstance()->getNewMyCircle();
        MyPointItem *edgeItem = myCircle->getEdgeItem();

        edgeItem->setPoint(event->scenePos());
        edgeItem->setPos(edgeItem->getPoint());

        myCircle->setEdge(p);
        edgeItem->scene()->update();
        myCircle->updateRadius();

    // 环形
    } else if (m_isCreateConCir && m_isCreatingConCir) {
        QPointF p(event->scenePos().x(), event->scenePos().y());
        MyConcentricCircle *myConCircle = BottomBar::getInstance()->getNewMyConCircle();
        MyPointItem *edgeItem = myConCircle->getEdgeItem();
        MyPointItem *anotherEdgeItem = myConCircle->getAnotherEdgeItem();

        edgeItem->setPoint(event->scenePos());
        edgeItem->setPos(edgeItem->getPoint());

        anotherEdgeItem->setPoint(QPointF(event->scenePos().x() + 20, event->scenePos().y() + 20));
        anotherEdgeItem->setPos(anotherEdgeItem->getPoint());

        myConCircle->setEdge(p);
        myConCircle->setAnotherEdge(QPointF(p.x() + 20, p.y() + 20));
        edgeItem->scene()->update();
        anotherEdgeItem->scene()->update();
        myConCircle->updateRadius();
        myConCircle->updateOtherRadius();
    }
    else {
        QGraphicsScene::mouseMoveEvent(event);
    }
}

void MyGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // 曲线
    if (m_isCreateCurve) {
        m_isPauseCurve = true;

    // 矩形
    } else if (m_isCreatingRect) {
        finishCreateRect();

    // 圆形
    } else if (m_isCreatingCircle) {
        finishCreateCircle();

    // 环形
    } else if (m_isCreateConCir) {
        finishCreateConCircle();
    }

    QGraphicsScene::mouseReleaseEvent(event);
}
