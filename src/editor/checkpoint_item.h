#pragma once

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include "editor/map_item.h"

class QGraphicsSceneWheelEvent;

// Claves para almacenar datos en QGraphicsItem
enum ItemDataKey {
    ItemTypeKey = 0,       // Tipo de item (checkpoint)
    ItemIdKey = 1,         // ID del item
    CheckpointTypeKey = 2  // Tipo de checkpoint (start/normal/finish)
};

// Valores para ItemTypeKey
enum ItemTypeValue { CheckpointItemType = 100 };

class CheckpointItem : public MapItem {
   public:
    enum CheckpointType {
        Start,   // Checkpoint de inicio (verde)
        Normal,  // Checkpoint intermedio (amarillo)
        Finish   // Checkpoint de meta (rojo)
    };

    CheckpointItem(int id, CheckpointType type, QPointF position,
                   float rotation = 0.0f, float width = 100.0f);

    // Métodos de QGraphicsItem
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    // Getters y setters
    int getId() const { return checkpointId; }
    CheckpointType getType() const { return type; }
    float getRotation() const { return rotation; }
    float getWidth() const { return width; }

    void setRotationAngle(float angle);
    void setWidth(float w);

   protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

   private:
    int checkpointId;
    CheckpointType type;
    float rotation;  // Ángulo en grados
    float width;     // Ancho de la franja
    bool dragging;
    QPointF dragStartPos;
    bool rotating;
    QPointF mouseStart;
    float rotationStart;

    QColor getColor() const;
    QString getLabel() const;
};
