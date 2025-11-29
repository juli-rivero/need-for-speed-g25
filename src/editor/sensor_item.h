#pragma once

#include <QBrush>
#include <QGraphicsItem>
#include <QPen>
#include <QPointF>
#include <QVector>

#include "editor/map_item.h"

class SensorItem : public MapItem {
   public:
    enum SensorType { Upper, Lower };

    explicit SensorItem(SensorType type);

    // Métodos para construcción por vértices
    void addVertex(const QPointF& pos);
    void finish();

    QVector<QPointF> getVertices() const { return vertices; }
    SensorType getSensorType() const { return type; }

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

   private:
    SensorType type;
    QVector<QPointF> vertices;
    bool finished;
};
