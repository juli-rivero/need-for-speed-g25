#pragma once

#include <QGraphicsItem>
#include <QList>
#include <QPainter>
#include <QPointF>

#include "editor/map_item.h"

class OverpassItem : public MapItem {
   public:
    explicit OverpassItem(QGraphicsItem* parent = nullptr);

    void addVertex(const QPointF& point);
    void finish();
    
    QList<QPointF> getVertices() const { return vertices; }
    bool isFinished() const { return finished; }

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

   private:
    QList<QPointF> vertices;
    bool finished;
};
