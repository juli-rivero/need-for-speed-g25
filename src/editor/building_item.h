#pragma once

#include <QColor>
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <QList>
#include <QPainter>
#include <QPointF>

#include "editor/map_item.h"

class BuildingItem : public MapItem {
   public:
    explicit BuildingItem(QGraphicsItem* parent = nullptr);

    void addVertex(const QPointF& point);
    void removeLastVertex();
    void finishBuilding();
    void clearVertices();
    void setVertices(const QList<QPointF>& verts);

    QList<QPointF> getVertices() const { return vertices; }
    bool isFinished() const { return finished; }

    // Getters/Setters para propiedades
    float getHeight() const { return height; }
    void setHeight(float h) { height = h; }

    QString getBuildingType() const { return buildingType; }
    void setBuildingType(const QString& type) { buildingType = type; }

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

   protected:
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant& value) override;

   private:
    void updatePolygon();
    void updateAppearance();
    QColor getBuildingColor() const;

    // Graham Scan para polígono convexo
    qreal crossProduct(const QPointF& o, const QPointF& a,
                       const QPointF& b) const;
    QList<QPointF> computeConvexHull(const QList<QPointF>& points) const;

    QList<QPointF> vertices;
    bool finished;
    float height;
    QString buildingType;
};

// Clase auxiliar para vértices editables
class BuildingVertexItem : public QGraphicsEllipseItem {
   public:
    BuildingVertexItem(int index, const QPointF& pos,
                       QGraphicsItem* parent = nullptr);
    int getIndex() const { return vertexIndex; }

   protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

   private:
    int vertexIndex;
};
