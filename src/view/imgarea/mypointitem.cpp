#include <QDebug>
#include <QtMath>

#include "mypointitem.h"
#include "mygraphicsitem.h"

MyPointItem::MyPointItem(QAbstractGraphicsShapeItem* parent, QPointF p, PointType type)
    : QAbstractGraphicsShapeItem(parent),
      m_point(p),
      m_type(type)
{
    this->setPos(m_point);
    this->setFlags(QGraphicsItem::ItemIsSelectable |
                   QGraphicsItem::ItemIsMovable |
                   QGraphicsItem::ItemIsFocusable);

    switch (type) {
    case Center:
        this->setCursor(Qt::OpenHandCursor);
        break;
    case Edge:
        this->setCursor(Qt::PointingHandCursor);
        break;
    case Special:
        this->setCursor(Qt::PointingHandCursor);
        break;
    default:
        break;
    }

}

MyPointItem::~MyPointItem()
{

}

QPointF MyPointItem::getPoint()
{
    return m_point;
}

void MyPointItem::setPoint(QPointF p)
{
    m_point = p;
}

void MyPointItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    return QAbstractGraphicsShapeItem::mousePressEvent(event);
}

QRectF MyPointItem::boundingRect() const
{
    return QRectF(-5, -5, 10, 10);
}

void MyPointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(this->pen());
    painter->setBrush(this->brush());
    this->setPos(m_point);

    switch (m_type) {
    case Center:
        painter->drawEllipse(-4, -4, 8, 8);
        break;
    case Edge:
        painter->drawRect(QRectF(-5, -5, 10, 10));
        break;
    case Special:
        painter->drawRect(QRectF(-5, -5, 10, 10));
        break;
    default:
        break;
    }
}

void MyPointItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton) {
        qreal disx = event->scenePos().x() - event->lastScenePos().x();
        qreal disy = event->scenePos().y() - event->lastScenePos().y();

        MyGraphicsItem *item = static_cast<MyGraphicsItem *>(this->parentItem());
        MyGraphicsItem::ItemType itemType = item->getType();

        switch (m_type) {
        case Center: {
            item->moveBy(disx, disy);
            this->setPos(m_point);
            this->scene()->update();
        }
            break;
        case Edge: {
            m_point = this->mapToParent(event->pos());
            this->setPos(m_point);
            this->scene()->update();

            switch (itemType) {
            case MyGraphicsItem::ItemType::Ellipse: {
                MyEllipse *ellipse = dynamic_cast<MyEllipse *>(item);
                ellipse->setEdge(m_point);
            }
                break;
            case MyGraphicsItem::ItemType::Circle: {
                MyCircle *circle = dynamic_cast<MyCircle *>(item);
                circle->setEdge(m_point);
                circle->updateRadius();
            }
                break;
            case MyGraphicsItem::ItemType::Rectangle: {
                MyGraphicsItem *rectangle = dynamic_cast<MyRectangle *>(item);
                rectangle->setEdge(m_point);
            }
                break;
            case MyGraphicsItem::ItemType::Polygon_Mask:
            case MyGraphicsItem::ItemType::Polygon: {
                MyPolygon *polygon = dynamic_cast<MyPolygon *>(item);
                polygon->updatePolygon(QPointF(event->lastScenePos().x(), event->lastScenePos().y()),
                                       QPointF(event->scenePos().x(), event->scenePos().y()));
            }
                break;
            case MyGraphicsItem::ItemType::Concentric_Circle: {
                MyCircle *circle = dynamic_cast<MyCircle *>(item);
                circle->setEdge(m_point);
                circle->updateRadius();
            }
                break;
            case MyGraphicsItem::ItemType::Curve: {
                MyCurve *curve = dynamic_cast<MyCurve *>(item);
                curve->updateCurve(QPointF(event->lastScenePos().x(), event->lastScenePos().y()),
                                   QPointF(event->scenePos().x(), event->scenePos().y()));
            }
                break;
            default:
                break;
            }
            break;
        }
            break;
        case Special: {
            switch (itemType) {
            case MyGraphicsItem::ItemType::Concentric_Circle: {
                m_point = this->mapToParent((event->pos()));
                this->setPos(m_point);
                this->scene()->update();

                MyConcentricCircle *concircle = dynamic_cast<MyConcentricCircle *>(item);
                concircle->setAnotherEdge(m_point);
                concircle->updateOtherRadius();
            }
                break;
            default:
                break;
            }
        }
            break;
        default:
            break;
        }
    }
}


void MyPointItemList::setRandColor()
{
    this->setColor(QColor(qrand() % 256, qrand() % 256, qrand() % 256));
}

void MyPointItemList::setColor(const QColor color)
{
    for (auto &temp : *this) {
        temp->setBrush(QBrush(color));
    }
}

void MyPointItemList::setVisible(bool visible)
{
    for (auto &temp : *this) {
        temp->setVisible(visible);
    }
}
