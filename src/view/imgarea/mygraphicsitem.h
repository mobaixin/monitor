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
        Circle = 11,         // 圆
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
    ~MyGraphicsItem();

    // 中心点
    QPointF getCenter() { return m_center; }
    void setCenter(QPointF p) { m_center = p; }

    // 边缘点
    QPointF getEdge() { return m_edge; }
    void setEdge(QPointF p) { m_edge = p; }

    // 获取类型
    ItemType getType() { return m_type; }

    // 获取真正的中心点坐标
    QPointF getRealCenter() { return scenePos() + boundingRect().center(); }

    // 获取边缘点列表
    QList<QPointF> getMyPointList();

    // 精确度
    int getAccuracy() { return m_accuracy; }
    void setAccuracy(int acc) { m_accuracy = acc; }

    // 像素值
    int getPixel() { return m_pixel; }
    void setPixel(int pix) { m_pixel = pix; }

    QPointF m_center;   // 中心点
    QPointF m_edge;     // 边缘点
    QList<QPointF> m_myPointList;   // 边缘点列表

    ItemType m_type;    // 类型
    MyPointItemList m_pointList;    // 功能点列表

    int m_accuracy;     // 精确度
    int m_pixel;        // 像素值

    QPen m_penIsSelected;   // 选中时画笔颜色
    QPen m_penNoSelected;   // 未选中时画笔颜色
    QPen m_penMaskArea;     // 屏蔽区画笔颜色

protected:
    // 焦点事件 选中和未选中
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;

//    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    // 绘制事件
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;
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

// 圆形
class MyCircle : public MyEllipse
{
public:
    MyCircle(qreal x, qreal y, qreal radius, ItemType type);

    enum {Type = 21};
    int type() const {
        return Type;
    }

    // 更新半径
    void updateRadius();

    // 获取边缘功能点
    MyPointItem *getEdgeItem();

protected:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;
//    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

public:
    qreal m_radius; // 半径
};

// 同心圆 圆环 环形
class MyConcentricCircle : public MyCircle
{
public:
    MyConcentricCircle(qreal x, qreal y, qreal radius1, qreal radius2, ItemType type);
    enum { Type = 22 };
    int type() const {
        return Type;
    }

    // 更新第二个半径
    void updateOtherRadius();

    // 设置第二个边缘点
    void setAnotherEdge(QPointF p);

    // 获取第二个边缘功能点
    MyPointItem *getAnotherEdgeItem();

protected:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

public:
    QPointF m_anotherEdge;  // 第二个边缘点
    qreal m_anotherRadius;  // 第二个半径
};

// 矩形
class MyRectangle : public MyGraphicsItem
{
public:
    MyRectangle(qreal x, qreal y, qreal width, qreal height, ItemType type);

    // 获取矩形边长
    qreal getRectLength(QString type);

    // 设置中心点和宽高
    void setRect(qreal x, qreal y, qreal width, qreal height);

    enum { Type = 23 };
    int type() const {
        return Type;
    }

    // 获取边缘功能点
    MyPointItem *getEdgeItem();

protected:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

public:
    qreal m_width;  // 宽度
    qreal m_height; // 高度
};


// 多边形
class MyPolygon : public MyGraphicsItem
{
    Q_OBJECT
public:
    MyPolygon(ItemType type);
//    ~MyPolygon();
    enum { Type = 28 };
    int type() const {
        return Type;
    }

    // 获取中心点
    QPointF getCentroid(QList<QPointF> list);

    // 获取边缘点到中心点的最大距离
    void getMaxLength();

    // 更新边缘点的坐标
    void updatePolygon(QPointF origin, QPointF end);

    // 判断点是否在边上
    bool isPointOnLine(QPointF point);

    // 是否是屏蔽区
    void setMask(bool isMask);
    bool getMask();

    // 设置边缘点
    void setPointList(QList<QPointF> edgePointList);

public slots:
    // 添加边缘点
    void pushPoint(QPointF p, QList<QPointF> list, bool isCenter);

protected:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

protected:
    qreal m_radius;             // 半径 即边缘点到中心点的最大距离
    bool m_isCreateFinished;    // 判断创建完成
    bool m_isMaskArea;          // 判断屏蔽区
    bool m_isAddPoint;          // 判断添加点
    bool m_isAddedPoint;        // 判断已添加点
    int  m_addPoingIdx;         // 添加点的位置

    MyPointItem *m_newPoint;    // 新边缘功能点
};


// 曲线
class MyCurve : public MyGraphicsItem
{
    Q_OBJECT
public:
    MyCurve(ItemType type);
    ~MyCurve();

    enum { Type = 29 };
    int type() const {
        return Type;
    }

    // 获取中心点
    QPointF getCentroid(QList<QPointF> list);

    // 获取边缘点到中心点的最大距离
    void getMaxLength();

    // 更新边缘点的坐标
    void updateCurve(QPointF origin, QPointF end);

    // 设置边缘点
    void setPointList(QList<QPointF> edgePointList);

public slots:
    // 添加边缘点
    void pushPoint(QPointF p, QList<QPointF> list, bool isCenter);

protected:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

protected:
    qreal m_radius;             // 半径 即边缘点到中心点的最大距离
    bool m_isCreateFinished;    // 判断创建完成
};

#endif // MYGRAPHICSITEM_H
