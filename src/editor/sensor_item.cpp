#include "editor/sensor_item.h"

#include <QPainter>
#include <QPolygonF>

SensorItem::SensorItem(SensorType type)
    : MapItem(), type(type), finished(false) {
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
}

void SensorItem::addVertex(const QPointF& pos) {
    if (finished) return;
    vertices.append(pos);
    prepareGeometryChange();
    update();
}

void SensorItem::finish() {
    if (vertices.size() >= 3) {
        finished = true;
        update();
    }
}

QRectF SensorItem::boundingRect() const {
    if (vertices.isEmpty()) return QRectF();
    return QPolygonF(vertices).boundingRect().adjusted(-5, -5, 5, 5);
}

void SensorItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*,
                       QWidget*) {
    if (vertices.isEmpty()) return;

    QColor color =
        (type == Upper) ? QColor(0, 255, 255, 80) : QColor(255, 0, 255, 80);
    QColor stroke = (type == Upper) ? Qt::cyan : Qt::magenta;

    if (isSelected()) stroke = Qt::yellow;

    QPen pen(stroke, 2, Qt::DashLine);
    painter->setPen(pen);
    painter->setBrush(color);

    QPolygonF poly(vertices);
    painter->drawPolygon(poly);

    if (!finished) {
        painter->setPen(Qt::white);
        painter->setBrush(Qt::black);
        for (const QPointF& p : vertices) {
            painter->drawEllipse(p, 3, 3);
        }
    } else {
        painter->setPen(Qt::black);
        painter->drawText(boundingRect(), Qt::AlignCenter,
                          (type == Upper) ? "UPPER" : "LOWER");
    }
}
