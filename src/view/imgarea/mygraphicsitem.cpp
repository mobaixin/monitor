#include <QVector>
#include <QDebug>
#include <QMenu>
#include <QSpinBox>
#include <QWidgetAction>
#include <QtMath>
#include <QApplication>

#include "mygraphicsitem.h"
#include "src/view/bottombar/bottombar.h"

MyGraphicsItem::MyGraphicsItem(QPointF center, QPointF edge, MyGraphicsItem::ItemType type)
    : m_center(center), m_edge(edge), m_type(type)
{
    m_penNoSelected.setColor(QColor(0, 160, 230));
    m_penNoSelected.setWidth(2);
    m_penIsSelected.setColor(QColor(255, 0, 255));
    m_penIsSelected.setWidth(2);
    m_penMaskArea.setColor(QColor(106, 8, 136));
    m_penMaskArea.setWidth(2);

    this->setPen(m_penNoSelected);
    this->setFlags(QGraphicsItem::ItemIsSelectable  |
                   QGraphicsItem::ItemIsMovable     |
                   QGraphicsItem::ItemIsFocusable);
}

MyGraphicsItem::~MyGraphicsItem()
{

}

QList<QPointF> MyGraphicsItem::getMyPointList()
{
    QList<QPointF> pointList;

    for (int i = 0; i < m_pointList.size(); i++) {
        pointList.append(m_pointList[i]->getPoint());
    }
    return pointList;
}

void MyGraphicsItem::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    this->setPen(m_penIsSelected);

    BottomBar::getInstance()->setAccuracy(m_accuracy);
    BottomBar::getInstance()->setPixel(m_pixel);

//    return QGraphicsItem::focusInEvent(event);
}

void MyGraphicsItem::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    this->setPen(m_penNoSelected);

    // 屏蔽区
    if (m_accuracy == -1) {
        this->setPen(m_penMaskArea);
    }

//    return QGraphicsItem::focusOutEvent(event);
}

//void MyGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
//{
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

//    return QAbstractGraphicsShapeItem::mouseMoveEvent(event);
//}

void MyGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (m_accuracy != -1) {
        //设置笔的颜色，和笔刷颜色
        painter->setPen(this->pen());
        painter->setBrush(this->brush());

        QFont font = painter->font();
        font.setPixelSize(18);
        painter->setFont(font);

        // 精确度和像素值文字
        painter->drawText(m_center.x() - 40, m_center.y(), QString("(%1, %2)").arg(m_accuracy).arg(m_pixel));
    }
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

MyPointItem *MyCircle::getEdgeItem()
{
    return m_pointList[0];
}

QRectF MyCircle::boundingRect() const
{
    int fixedRadius = m_radius > 100 ? m_radius : 100;
    return QRectF(m_center.x()- fixedRadius, m_center.y() - fixedRadius, fixedRadius * 2, fixedRadius * 2);
}

void MyCircle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(this->pen());
    painter->setBrush(this->brush());

    QRectF rect(m_center.x() - m_radius, m_center.y() - m_radius, m_radius * 2, m_radius * 2);
    painter->drawEllipse(rect);

    return MyGraphicsItem::paint(painter, option, widget);
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

MyPointItem *MyConcentricCircle::getAnotherEdgeItem()
{
    // 0和2是edge
    return m_pointList[2];
}

QRectF MyConcentricCircle::boundingRect() const
{
    qreal maxRadius = m_radius > m_anotherRadius ? m_radius : m_anotherRadius;
    maxRadius = maxRadius > 100 ? maxRadius : 100;

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

MyPointItem *MyRectangle::getEdgeItem()
{
    return m_pointList[0];
}

QRectF MyRectangle::boundingRect() const
{
    int fixedWid = m_width > 200 ? m_width : 200;
    int fixedHei = m_height > 60 ? m_height : 60;

    return QRectF(m_center.x() - fixedWid/2 - 5, m_center.y() - fixedHei/2 - 5,
                  fixedWid + 10, fixedHei + 10);
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

    return MyGraphicsItem::paint(painter, option, widget);
}

// 多边形
MyPolygon::MyPolygon(MyGraphicsItem::ItemType type)
    : MyGraphicsItem(QPointF(0, 0), QPointF(0, 0), type)
{
    m_isCreateFinished = false;
    m_isAddPoint = false;
    m_isAddedPoint = false;

    if (type == MyGraphicsItem::ItemType::Polygon_Mask) {
        this->setPen(m_penMaskArea);
    }
}

//MyPolygon::~MyPolygon()
//{
//    qDebug() << "~MyPolygon";

////    for (int i = 0; i < m_pointList.size(); i++) {
////        if (m_pointList[i] != nullptr) {
////            m_pointList[i]->deleteLater();
////        }
////    }
//}

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

        m_isAddedPoint = true;
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

void MyPolygon::setPointList(QList<QPointF> edgePointList)
{
    if (edgePointList.size() < 3) {
        return ;
    }

    for (int i = 0; i < edgePointList.size(); i++) {
        MyPointItem *point = new MyPointItem(this, edgePointList[i], MyPointItem::Edge);
        point->setParentItem(this);
        m_pointList.append(point);
    }

    m_center = getCentroid(edgePointList);
    MyPointItem *point = new MyPointItem(this, m_center, MyPointItem::Center);
    point->setParentItem(this);
    m_pointList.append(point);
    m_pointList.setRandColor();

    m_isCreateFinished = true;
    getMaxLength();
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
            MyPointItem *point = new MyPointItem(this, m_center, MyPointItem::Center);
            point->setParentItem(this);
            m_pointList.append(point);
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

//            m_pointList.setColor(QColor(0, 255, 0));
            m_pointList.setRandColor();
        }

        this->update();
    }
}

QRectF MyPolygon::boundingRect() const
{
    int fixedRadius = m_radius > 100 ? m_radius : 100;
    return QRectF(m_center.x() - fixedRadius, m_center.y() - fixedRadius, fixedRadius * 2, fixedRadius * 2);
}

void MyPolygon::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(this->pen());
    painter->setBrush(this->brush());

    if (m_isCreateFinished) {
//        qDebug() << "finish create";
        for (int i = 1; i < m_pointList.size() - 1; i++) {
//            qDebug() << m_pointList.at(i)->getPoint().x() << ", " << m_pointList.at(i)->getPoint().y();
            if (m_pointList.at(i)->getPoint().x() < 1 || m_pointList.at(i)->getPoint().y() < 1) {
                return ;
            }

            painter->drawLine(m_pointList.at(i - 1)->getPoint(), m_pointList.at(i)->getPoint());
//            qDebug() << m_pointList.at(i - 1)->getPoint().x() << ", " << m_pointList.at(i - 1)->getPoint().y();
        }
//        qDebug() << m_pointList.at(m_pointList.size() - 2)->getPoint().x() << ", " << m_pointList.at(m_pointList.size() - 2)->getPoint().y();

        painter->drawLine(m_pointList.at(m_pointList.size()-2)->getPoint(), m_pointList.at(0)->getPoint());
    } else {
//        qDebug() << "is creating";
        for (int i = 1; i < m_pointList.size(); i++) {
            painter->drawLine(m_pointList.at(i - 1)->getPoint(), m_pointList.at(i)->getPoint());
//            qDebug() << m_pointList.at(i - 1)->getPoint().x() << ", " << m_pointList.at(i - 1)->getPoint().y();
        }
//        if (m_pointList.size() > 0) {
//            qDebug() << m_pointList.at(m_pointList.size() - 1)->getPoint().x() << ", " << m_pointList.at(m_pointList.size() - 1)->getPoint().y();
//        }

    }

    if (m_isCreateFinished) {
        return MyGraphicsItem::paint(painter, option, widget);
    }
}

void MyPolygon::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
//    qDebug() << "MyPolygon::mousePressEvent";
    QPointF point = event->pos();
    isPointOnLine(point);

    return QGraphicsItem::mousePressEvent(event);
}

void MyPolygon::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_isAddedPoint && event->buttons() == Qt::LeftButton) {
//        qreal disx = event->scenePos().x() - event->lastScenePos().x();
//        qreal disy = event->scenePos().y() - event->lastScenePos().y();

//        qDebug() << " MyPolygon::mouseMoveEvent";
//        qDebug() << "m_newPoint: " << m_newPoint->x() << " " << m_newPoint->y();
//        qDebug() << "event->pos(): " << event->pos().x() << " " << event->pos().y();
        m_newPoint->setPoint(event->pos());
        m_newPoint->setPos(m_newPoint->getPoint());
        m_newPoint->scene()->update();
    } else {
        return QGraphicsItem::mouseMoveEvent(event);
    }


}

void MyPolygon::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    m_isAddedPoint = false;
    return QGraphicsItem::mouseReleaseEvent(event);
}

MyCurve::MyCurve(MyGraphicsItem::ItemType type)
    : MyGraphicsItem(QPointF(0, 0), QPointF(0, 0), type)
{
    m_isCreateFinished = false;
}

MyCurve::~MyCurve()
{

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

void MyCurve::setPointList(QList<QPointF> edgePointList)
{
    if (edgePointList.size() < 3) {
        return ;
    }

    for (int i = 0; i < edgePointList.size(); i++) {
        MyPointItem *point = new MyPointItem(this, edgePointList[i], MyPointItem::Other);
        point->setParentItem(this);
        m_pointList.append(point);
    }

    m_center = getCentroid(edgePointList);
    MyPointItem *point = new MyPointItem(this, m_center, MyPointItem::Center);
    point->setParentItem(this);
    m_pointList.append(point);
    m_pointList.setRandColor();

    m_isCreateFinished = true;
    getMaxLength();
}

void MyCurve::pushPoint(QPointF p, QList<QPointF> list, bool isCenter)
{
    if (!m_isCreateFinished) {
        m_center = getCentroid(list);
        getMaxLength();

        if (isCenter) {
            MyPointItem *point = new MyPointItem(this, m_center, MyPointItem::Center);
            point->setParentItem(this);
            m_pointList.append(point);
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
    int fixedRadius = m_radius > 100 ? m_radius : 100;
    return QRectF(m_center.x() - fixedRadius, m_center.y() - fixedRadius, fixedRadius * 2, fixedRadius * 2);
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

    if (m_isCreateFinished) {
        return MyGraphicsItem::paint(painter, option, widget);
    }
}
