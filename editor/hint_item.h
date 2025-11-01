#ifndef HINT_ITEM_H
#define HINT_ITEM_H

#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include "checkpoint_item.h"  // Para ItemDataKey e ItemTypeValue

/**
 * Item gráfico para representar una flecha de ayuda (hint).
 * Las flechas indican a los jugadores hacia dónde deben ir.
 */
class HintItem : public QGraphicsItem {
public:
    HintItem(int id, QPointF position, float rotation = 0.0f, int targetCheckpoint = -1);
    
    // Métodos de QGraphicsItem
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, 
               QWidget* widget = nullptr) override;
    
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
    float rotation;  // Dirección de la flecha en grados
    int targetCheckpoint;  // ID del checkpoint al que apunta
    bool dragging;
    QPointF dragStartPos;
};

#endif // HINT_ITEM_H
