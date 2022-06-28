#include <QVector>
#include <QDebug>
#include <QMenu>
#include <QSpinBox>
#include <QWidgetAction>
#include <QtMath>
#include <QApplication>

#include "mygraphicsitem.h"

MyGraphicsItem::MyGraphicsItem(QPointF center, QPointF edge, MyGraphicsItem::ItemType type)
    : m_center(center), m_edge(edge), m_type(type)
{
    m_penNoSelected.setColor(QColor(0, 160, 230));
    m_penNoSelected.setWidth(2);
    m_penIsSelected.setColor(QColor(255, 0, 255));
    m_penIsSelected.setWidth(2);

    this->setPen(m_penNoSelected);
    this->setFlags(QGraphicsItem::ItemIsSelectable  |
                   QGraphicsItem::ItemIsMovable     |
                   QGraphicsItem::ItemIsFocusable);
}

QList<QPointF> MyGraphicsItem::getMyPointList()
{
    QList<QPointF> pointList;

    for (int i = 0; i < m_pointList.size() - 1; i++) {
        pointList.append(m_pointList[i]->getPoint());
    }
    return pointList;
}

void MyGraphicsItem::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    this->setPen(m_penIsSelected);
}

void MyGraphicsItem::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    this->setPen(m_penNoSelected);
}

void MyGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
//    qDebug() << "MyGraphicsItem::mouseMoveEvent";

//    if (event->buttons() == Qt::LeftButton) {
//        qreal disx = event->scenePos().x() - event->lastScenePos().x();
//        qreal disy = event->scenePos().y() - event->lastScenePos().y();

////        this->setCenter(QPointF(m_center.x() + disx, m_center.y() + disy));
////        this->setEdge(this->mapToParent(QPointF(m_edge.x() + disx, m_edge.y() + disy)));
//        //获得item在scene中的逻辑位置
//        QPointF scene_pos = this->scenePos();
//        //或者找到item的中心点在scene中位置
//        QPointF scenecenter_pos = this->scenePos() + boundingRect().center();
//        this->setCenter(scenecenter_pos);

//        qDebug() << "scene pos: " << scene_pos.x() << " " << scene_pos.y();
//        qDebug() << "scene center pos: " << scenecenter_pos.x() << " " << scenecenter_pos.y();
//    }

//    qDebug() << "real center:" << getRealCenter().x() << " " << getRealCenter().y();
//    qDebug() << "center:" << getCenter().x() << " " << getCenter().y();
//    qDebug() << "edge:" << getEdge().x() << " " << getEdge().y();

//    for (MyPointItem *item : m_pointList) {
//        qDebug() << "item:" << item->getPoint().x() << " " << item->getPoint().y();
//    }

    return QAbstractGraphicsShapeItem::mouseMoveEvent(event);
}


MyEllipse::MyEllipse(qreal x, qreal y, qreal width, qreal height, MyGraphicsItem::ItemType type)
    : MyGraphicsItem(QPointF(x, y), QPointF(x + width / 2, y+ height/2), type)
{
    MyPointItem *pointItem = new MyPointItem(this, m_edge, MyPointItem::Edge);
    pointItem->setParentItem(this);
    m_pointList.append(pointItem);
    m_pointList.append(new MyPointItem(this, m_center, MyPointItem::Center));
    m_pointList.setRandColor();
}

QRectF MyEllipse::boundingRect() const
{
    return QRectF(m_center.x() - abs(int(m_edge.x())), m_center.y() - abs(int(m_edge.y())),
                  abs(abs(m_edge.x())) * 2, abs(int(m_edge.y()))* 2);
}

void MyEllipse::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(this->pen());
    painter->setBrush(this->brush());

    QRectF rect(m_center.x() - abs(int(m_edge.x())), m_center.y() - abs(int(m_edge.y())),
               abs(int(m_edge.x())) * 2, abs(int(m_edge.y())) * 2);
    painter->drawEllipse(rect);
}

//void MyEllipse::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
//{

//}

// 圆
MyCircle::MyCircle(qreal x, qreal y, qreal radius, MyGraphicsItem::ItemType type)
    : MyEllipse(x, y, radius * sqrt(2), radius * sqrt(2), type)
{
    updateRadius();
}

void MyCircle::updateRadius()
{
    m_radius = sqrt(pow(m_center.x() - m_edge.x(), 2) + pow(m_center.y() - m_edge.y(), 2));
}

QRectF MyCircle::boundingRect() const
{
    return QRectF(m_center.x()- m_radius, m_center.y() - m_radius, m_radius * 2, m_radius * 2);
}

void MyCircle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(this->pen());
    painter->setBrush(this->brush());

    QRectF rect(m_center.x() - m_radius, m_center.y() - m_radius, m_radius * 2, m_radius * 2);
    painter->drawEllipse(rect);
}

//void MyCircle::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
//{

//}

// 同心圆
MyConcentricCircle::MyConcentricCircle(qreal x, qreal y, qreal radius1, qreal radius2, MyGraphicsItem::ItemType type)
    : MyCircle(x, y, radius1, type),
      m_anotherEdge(x + radius2*sqrt(2)/2, y + radius2*sqrt(2)/2)
{
    MyPointItem *point = new MyPointItem(this, m_anotherEdge, MyPointItem::Special);
    point->setParentItem(this);
    m_pointList.append(point);
    m_pointList.setRandColor();

    updateOtherRadius();
}

void MyConcentricCircle::updateOtherRadius()
{
    m_anotherRadius = sqrt(pow(m_center.x() - m_anotherEdge.x(), 2) +
                           pow(m_center.y() - m_anotherEdge.y(), 2));
}

void MyConcentricCircle::setAnotherEdge(QPointF p)
{
    m_anotherEdge = p;
}

QRectF MyConcentricCircle::boundingRect() const
{
    qreal maxRadius = m_radius > m_anotherRadius ? m_radius : m_anotherRadius;
    return QRectF(m_center.x() - maxRadius, m_center.y() - maxRadius,
                  maxRadius * 2, maxRadius * 2);
}

void MyConcentricCircle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(this->pen());
    painter->setBrush(this->brush());

    QRectF rect(m_center.x() - m_anotherRadius, m_center.y() - m_anotherRadius,
                m_anotherRadius * 2, m_anotherRadius * 2);
    painter->drawEllipse(rect);

    MyCircle::paint(painter, option, widget);
}


// 矩形
MyRectangle::MyRectangle(qreal x, qreal y, qreal width, qreal height, MyGraphicsItem::ItemType type)
    : MyGraphicsItem(QPointF(x, y), QPointF(x+width/2, y+height/2), type)
{
    MyPointItem *edgePoint   = new MyPointItem(this, m_edge, MyPointItem::Edge);
    edgePoint->setParentItem(this);
    m_pointList.append(edgePoint);

    MyPointItem *centerPoint = new MyPointItem(this, m_center, MyPointItem::Center);
    m_pointList.append(centerPoint);
    m_pointList.setRandColor();

    m_width  = width;
    m_height = height;
}

qreal MyRectangle::getRectLength(QString type)
{
    qreal rectWidth, rectHeight;
    rectWidth  = abs(int(m_edge.x() - m_center.x()) * 2);
    rectHeight = abs(int(m_edge.y() - m_center.y()) * 2);

    if (type == "width")
        return rectWidth;
    else
        return rectHeight;
}

void MyRectangle::setRect(qreal x, qreal y, qreal width, qreal height)
{
    m_center.setX(x);
    m_center.setY(y);
    m_edge.setX(x + width / 2);
    m_edge.setY(y + height / 2);
    m_pointList.at(0)->setPoint(m_edge);

    m_width  = width;
    m_height = height;

    this->hide();
    this->update();
    this->show();
}

QRectF MyRectangle::boundingRect() const
{
    return QRectF(m_center.x() - m_width/2 - 5, m_center.y() - m_height/2 - 5,
                  m_width + 10, m_height + 10);
}

void MyRectangle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(this->pen());
//    painter->setBrush(this->brush());

    m_width  = abs(int(m_edge.x() - m_center.x()) * 2);
    m_height = abs(int(m_edge.y() - m_center.y()) * 2);

    QRectF rect(m_center.x() - m_width/2, m_center.y() - m_height/2, m_width, m_height);
    painter->drawRect(rect);
}





// 多边形
MyPolygon::MyPolygon(MyGraphicsItem::ItemType type)
    : MyGraphicsItem(QPointF(0, 0), QPointF(0, 0), type)
{
    m_isCreateFinished = false;
    m_isAddPoint = false;
}

QPointF MyPolygon::getCentroid(QList<QPointF> list)
{
    qreal x = 0;
    qreal y = 0;
    for(auto &temp : list) {
        x += temp.x();
        y += temp.y();
    }
    x = x / list.size();
    y = y / list.size();
    return QPointF(x, y);
}

void MyPolygon::getMaxLength()
{
    QVector<qreal> vector;
    for (auto &temp : m_pointList) {
        qreal dis = sqrt(pow(m_center.x() - temp->x(), 2) + pow(m_center.y() - temp->y(), 2));
        vector.append(dis);
    }

    qreal maxLen = 0;
    for (auto &temp : vector) {
        if (temp > maxLen) {
            maxLen = temp;
        }
    }
    m_radius = maxLen;
}

void MyPolygon::updatePolygon(QPointF origin, QPointF end)
{
    QList<QPointF> list;

    for (auto &temp : m_pointList) {
        if (temp->getPoint() == origin) {
            temp->setPoint(end);
        }
        list.append(temp->getPoint());
    }

    m_center = getCentroid(list);
    m_pointList.at(m_pointList.size() - 1)->setPoint(m_center);

    getMaxLength();
}

bool MyPolygon::isPointOnLine(QPointF point)
{
    bool flag = false;
    int i = 1;

//    qDebug() << "point: " << point.x() << point.y();

    for (i = 1; i < m_pointList.size(); i++) {
        QPointF startP = m_pointList.at(i - 1)->getPoint();
        QPointF endP   = m_pointList.at(i)->getPoint();
        if (i == m_pointList.size() - 1) {
            endP = m_pointList.at(0)->getPoint();
        }

        double length1 = sqrt(pow(startP.x() - endP.x(), 2) + pow(startP.y() - endP.y(), 2));
        double length2 = sqrt(pow(startP.x() - point.x(), 2) + pow(startP.y() - point.y(), 2));
        double length3 = sqrt(pow(endP.x() - point.x(), 2) + pow(endP.y() - point.y(), 2));
//        qDebug() << length2 - length1;
        if ((fabs(length2 + length3 - length1) < 2) && (length2 > 20) && (length3 > 20)) {
            flag = true;
            break;
        }
    }

    if (flag) {
        QList<QPointF> list;
        for (int j = 0; j < m_pointList.size() - 1; j++) {
            list.append(m_pointList.at(j)->getPoint());
        }
        m_isAddPoint = true;
        m_addPoingIdx = i;
        pushPoint(point, list, false);
        m_isAddPoint = false;
    }

//    qDebug() << "isPointOnLine: " << flag;

    return flag;
}

void MyPolygon::setMask(bool isMask)
{
    m_isMaskArea = isMask;
}

bool MyPolygon::getMask()
{
    return m_isMaskArea;
}

void MyPolygon::pushPoint(QPointF p, QList<QPointF> list, bool isCenter)
{
    if (!m_isCreateFinished || m_isAddPoint) {
        m_center = getCentroid(list);
        getMaxLength();

        if (isCenter) {
            if (m_pointList.size() < 3) {
                m_pointList.clear();
                m_isCreateFinished = true;
                this->deleteLater();
                return;
            }
            m_pointList.append(new MyPointItem(this, m_center, MyPointItem::Center));
            m_pointList.setRandColor();
            m_isCreateFinished = true;
        } else {
            MyPointItem *point = new MyPointItem(this, p, MyPointItem::Edge);
            point->setParentItem(this);

            m_newPoint = point;

            if (m_isAddPoint && m_addPoingIdx > 0) {
                m_pointList.insert(m_addPoingIdx, point);
            } else {
                m_pointList.append(point);
            }

            m_pointList.setColor(QColor(0, 255, 0));

            if (m_isAddPoint) {
                point->setAcceptedMouseButtons(Qt::LeftButton);
                point->setFocus();
                QMouseEvent mouseEvent( QEvent::MouseButtonPress, p, Qt::LeftButton , Qt::LeftButton,Qt::NoModifier );
                QMouseEvent mouseEvent_1( QEvent::MouseButtonRelease, p, Qt::LeftButton , Qt::LeftButton,Qt::NoModifier );
                QApplication::sendEvent( this, &mouseEvent_1 );
                QApplication::sendEvent( point, &mouseEvent );
            }
        }

        this->update();
    }
}

QRectF MyPolygon::boundingRect() const
{
    return QRectF(m_center.x() - m_radius, m_center.y() - m_radius, m_radius * 2, m_radius * 2);
}

void MyPolygon::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(this->pen());
    painter->setBrush(this->brush());

    if (m_isCreateFinished) {
        for (int i = 1; i < m_pointList.size() - 1; i++) {
            painter->drawLine(m_pointList.at(i - 1)->getPoint(), m_pointList.at(i)->getPoint());
        }

        painter->drawLine(m_pointList.at(m_pointList.size()-2)->getPoint(), m_pointList.at(0)->getPoint());
    } else {
        for (int i = 1; i < m_pointList.size(); i++) {
            painter->drawLine(m_pointList.at(i - 1)->getPoint(), m_pointList.at(i)->getPoint());
        }
    }
}

void MyPolygon::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
//    qDebug() << "MyPolygon::mousePressEvent";
    QPointF point = event->pos();
    isPointOnLine(point);

    return QAbstractGraphicsShapeItem::mousePressEvent(event);
}

//void MyPolygon::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
//{
//    return QAbstractGraphicsShapeItem::mouseMoveEvent(event);
//}

MyCurve::MyCurve(MyGraphicsItem::ItemType type)
    : MyGraphicsItem(QPointF(0, 0), QPointF(0, 0), type)
{
    m_isCreateFinished = false;
}

QPointF MyCurve::getCentroid(QList<QPointF> list)
{
    qreal x = 0;
    qreal y = 0;

    for (auto &temp : list) {
        x += temp.x();
        y += temp.y();
    }

    x = x / list.size();
    y = y / list.size();

    return QPointF(x, y);
}

void MyCurve::getMaxLength()
{
    qreal maxLen = 0;
    for (auto &temp : m_pointList) {
        qreal dis = sqrt(pow(m_center.x() - temp->x(), 2) + pow(m_center.y() - temp->y(), 2));

        if (dis > maxLen) {
            maxLen = dis;
        }
    }
    m_radius = maxLen;

}

void MyCurve::updateCurve(QPointF origin, QPointF end)
{
    QList<QPointF> list;

    for (auto &temp : m_pointList) {
        if (temp->getPoint() == origin) {
            temp->setPoint(end);
        }
        list.append(temp->getPoint());
    }

    m_center = getCentroid(list);
    m_pointList.at(m_pointList.size()-1)->setPoint(m_center);

    getMaxLength();
}

void MyCurve::pushPoint(QPointF p, QList<QPointF> list, bool isCenter)
{
    if (!m_isCreateFinished) {
        m_center = getCentroid(list);
        getMaxLength();

        if (isCenter) {
            m_pointList.append(new MyPointItem(this, m_center, MyPointItem::Center));
            m_pointList.setRandColor();
            m_isCreateFinished = true;
        } else {
            MyPointItem *point = new MyPointItem(this, p, MyPointItem::Other);
            point->setParentItem(this);
            m_pointList.append(point);
        }
        this->update();
    }
}

QRectF MyCurve::boundingRect() const
{
    return QRectF(m_center.x() - m_radius, m_center.y() - m_radius, m_radius * 2, m_radius * 2);
}

void MyCurve::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(this->pen());
    painter->setBrush(this->brush());

    if (m_isCreateFinished) {
        for (int i = 1; i < m_pointList.size() - 1; i++) {
            painter->drawLine(m_pointList.at(i - 1)->getPoint(), m_pointList.at(i)->getPoint());
        }
        painter->drawLine(m_pointList.at(m_pointList.size() - 2)->getPoint(), m_pointList.at(0)->getPoint());
    } else {
        for (int i = 1; i < m_pointList.size(); i++) {
            painter->drawLine(m_pointList.at(i - 1)->getPoint(), m_pointList.at(i)->getPoint());
        }
    }
}
