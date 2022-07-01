#ifndef MYGRAPHICSITEM_H
#define MYGRAPHICSITEM_H

#include <QObject>
#include <QMouseEvent>
#include "mypointitem.h"

#define PI 3.1415926

class MyGraphicsItem : public QObject, public QAbstractGraphicsShapeItem
{
    Q_OBJECT
public:
    enum ItemType {
        Circle = 0,         // 圆
        Ellipse,            // 椭圆
        Concentric_Circle,  // 同心圆
        Pie,                // 饼
        Chord,              // 和弦
        Rectangle,          // 矩形
        Square,             // 正方形
        Polygon,            // 多边形
        Polygon_Mask,       // 屏蔽区
        Round_End_Rectangle,// 圆端矩形
        Rounded_Rectangle,  // 圆角矩形
        Curve               // 曲线
    };

    MyGraphicsItem(QPointF center, QPointF edge, ItemType type);

    QPointF getCenter() { return m_center; }
    void setCenter(QPointF p) { m_center = p; }

    QPointF getEdge() { return m_edge; }
    void setEdge(QPointF p) { m_edge = p; }

    ItemType getType() { return m_type; }

    QPointF getRealCenter() { return scenePos() + boundingRect().center(); }
    QList<QPointF> getMyPointList();

    int getAccuracy() { return m_accuracy; }
    void setAccuracy(int acc) { m_accuracy = acc; }

    int getPixel() { return m_pixel; }
    void setPixel(int pix) { m_pixel = pix; }

protected:
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    QPointF m_center;
    QPointF m_edge;
    QList<QPointF> m_myPointList;

    ItemType m_type;
    MyPointItemList m_pointList;

    int m_accuracy;
    int m_pixel;

    QPen m_penIsSelected;
    QPen m_penNoSelected;
};

// 椭圆
class MyEllipse : public MyGraphicsItem
{
    Q_OBJECT
public:
    MyEllipse(qreal x, qreal y, qreal width, qreal height, ItemType type);

    enum {Type = 20};
    int type() const {
        return Type;
    }

protected:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;
//    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
};

// 圆
class MyCircle : public MyEllipse
{
public:
    MyCircle(qreal x, qreal y, qreal radius, ItemType type);

    enum {Type = 21};
    int type() const {
        return Type;
    }

    void updateRadius();

protected:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;
//    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

public:
    qreal m_radius;
};

// 同心圆
class MyConcentricCircle : public MyCircle
{
public:
    MyConcentricCircle(qreal x, qreal y, qreal radius1, qreal radius2, ItemType type);
    enum { Type = 22 };
    int type() const {
        return Type;
    }
    void updateOtherRadius();
    void setAnotherEdge(QPointF p);

protected:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

public:
    QPointF m_anotherEdge;
    qreal m_anotherRadius;
};

// 矩形
class MyRectangle : public MyGraphicsItem
{
public:
    MyRectangle(qreal x, qreal y, qreal width, qreal height, ItemType type);
    qreal getRectLength(QString type);
    void setRect(qreal x, qreal y, qreal width, qreal height);

    enum { Type = 23 };
    int type() const {
        return Type;
    }

protected:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

public:
    qreal m_width;
    qreal m_height;
};


// 多边形
class MyPolygon : public MyGraphicsItem
{
    Q_OBJECT
public:
    MyPolygon(ItemType type);
    enum { Type = 28 };
    int type() const {
        return Type;
    }

    QPointF getCentroid(QList<QPointF> list);
    void getMaxLength();
    void updatePolygon(QPointF origin, QPointF end);

    bool isPointOnLine(QPointF point);

    void setMask(bool isMask);
    bool getMask();

public slots:
    void pushPoint(QPointF p, QList<QPointF> list, bool isCenter);

protected:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
//    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

protected:
    qreal m_radius;
    bool m_isCreateFinished;
    bool m_isMaskArea;
    bool m_isAddPoint;
    int  m_addPoingIdx;

    MyPointItem *m_newPoint;
};


// 曲线
class MyCurve : public MyGraphicsItem
{
    Q_OBJECT
public:
    MyCurve(ItemType type);
    enum { Type = 29 };
    int type() const {
        return Type;
    }
    QPointF getCentroid(QList<QPointF> list);
    void getMaxLength();
    void updateCurve(QPointF origin, QPointF end);
//    void updateCurve(QPointF origin, QPointF mid, QPointF end);

public slots:
    void pushPoint(QPointF p, QList<QPointF> list, bool isCenter);

protected:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

protected:
    qreal m_radius;
    bool m_isCreateFinished;
};

#endif // MYGRAPHICSITEM_H
