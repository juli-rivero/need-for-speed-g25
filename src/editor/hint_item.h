#pragma once

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include "editor/checkpoint_item.h"
#include "editor/map_item.h"

class HintItem : public MapItem {
   public:
    HintItem(int id, QPointF position, float rotation = 0.0f,
             int targetCheckpoint = -1);

    // Métodos de QGraphicsItem
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    // Getters y setters
    int getId() const { return hintId; }
    float getRotation() const { return rotation; }
    int getTargetCheckpoint() const { return targetCheckpoint; }

    void setRotationAngle(float angle);
    void setTargetCheckpoint(int target);

   protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

   private:
    int hintId;
    float rotation;        // Dirección de la flecha en grados
    int targetCheckpoint;  // ID del checkpoint al que apunta
    bool dragging;
    QPointF dragStartPos;
};
