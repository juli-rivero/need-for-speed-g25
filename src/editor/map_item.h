#pragma once

#include <QGraphicsItem>

class MapItem : public QGraphicsItem {
   public:
    virtual ~MapItem() = default;

    // Métodos virtuales puros que deben implementar las clases derivadas
    QRectF boundingRect() const override = 0;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override = 0;
};
