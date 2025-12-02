#include "editor/overpass_item.h"

OverpassItem::OverpassItem(QGraphicsItem* parent) : MapItem(), finished(false) {
    Q_UNUSED(parent);
    setZValue(5);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

void OverpassItem::addVertex(const QPointF& point) {
    vertices.append(point);
    prepareGeometryChange();
    update();
}

void OverpassItem::finish() {
    finished = true;
    update();
}

QRectF OverpassItem::boundingRect() const {
    if (vertices.isEmpty()) return QRectF();

    qreal minX = vertices.first().x();
    qreal minY = vertices.first().y();
    qreal maxX = minX;
    qreal maxY = minY;

    for (const QPointF& p : vertices) {
        if (p.x() < minX) minX = p.x();
        if (p.x() > maxX) maxX = p.x();
        if (p.y() < minY) minY = p.y();
        if (p.y() > maxY) maxY = p.y();
    }

    return QRectF(minX, minY, maxX - minX, maxY - minY).adjusted(-2, -2, 2, 2);
}

QPainterPath OverpassItem::shape() const {
    QPainterPath path;
    if (vertices.size() > 2) {
        path.addPolygon(QPolygonF(vertices.toVector()));
    }
    return path;
}

void OverpassItem::paint(QPainter* painter,
                         const QStyleOptionGraphicsItem* option,
                         QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (vertices.isEmpty()) return;

    QPen pen(Qt::darkCyan, 2);
    if (isSelected()) {
        pen.setColor(Qt::yellow);
        pen.setStyle(Qt::DashLine);
    }
    painter->setPen(pen);

    QBrush brush(QColor(0, 139, 139, 100));
    painter->setBrush(brush);

    if (finished && vertices.size() >= 3) {
        painter->drawPolygon(vertices.toVector());
    } else {
        // Draw lines while creating
        if (vertices.size() > 1) {
            painter->drawPolyline(vertices.toVector());
        }
        // Draw points for vertices
        painter->setBrush(Qt::red);
        for (const QPointF& p : vertices) {
            painter->drawEllipse(p, 2, 2);
        }
    }
}
