#ifndef MAP_ITEM_H
#define MAP_ITEM_H

#include <QGraphicsItem>

class MapItem : public QGraphicsItem {
 public:
  virtual ~MapItem() = default;

  // Métodos virtuales puros que deben implementar las clases derivadas
  virtual QRectF boundingRect() const override = 0;
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                     QWidget* widget = nullptr) override = 0;
};

#endif
