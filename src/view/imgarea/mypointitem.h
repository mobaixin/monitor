#ifndef MYPOINTITEM_H
#define MYPOINTITEM_H

#include <QObject>
#include <QPointF>
#include <QPen>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QAbstractGraphicsShapeItem>
#include <QCursor>
#include <QKeyEvent>
#include <QList>

// 自定义点
class MyPointItem : public QObject, public QAbstractGraphicsShapeItem
{
    Q_OBJECT
public:
    enum PointType {
        Center = 1, // 中心点
        Edge,       // 边缘点（可拖动改变图形的形状、大小）
        Special,    // 特殊功能点
        Other       // 普通点
    };

    MyPointItem(QAbstractGraphicsShapeItem* parent, QPointF p, PointType type);
    ~MyPointItem();

    QPointF getPoint();
    void setPoint(QPointF p);

    PointType getPointType() { return m_type; }

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

protected:
    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QPointF m_point;
    PointType m_type;
};


// 存放点的容器
class MyPointItemList : public QList<MyPointItem *>
{
public:
    void setRandColor();
    void setColor(const QColor color);
    void setVisible(bool visible);
};

#endif // MYPOINTITEM_H
