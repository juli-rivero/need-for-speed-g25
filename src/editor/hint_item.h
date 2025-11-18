#ifndef HINT_ITEM_H
#define HINT_ITEM_H

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include "checkpoint_item.h"
#include "map_item.h"

class HintItem : public MapItem {
 public:
  HintItem(int id, QPointF position, float rotation = 0.0f,
           int targetCheckpoint = -1);

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
  float rotation;        // Dirección de la flecha en grados
  int targetCheckpoint;  // ID del checkpoint al que apunta
  bool dragging;
  QPointF dragStartPos;
};

#endif
