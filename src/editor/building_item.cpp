#include "editor/building_item.h"

#include <QBrush>
#include <QDebug>
#include <QPainterPath>
#include <QPen>
#include <algorithm>
#include <cmath>

BuildingItem::BuildingItem(QGraphicsItem* parent)
    : MapItem(), finished(false), height(10.0f), buildingType("residential") {
    Q_UNUSED(parent);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
}

void BuildingItem::addVertex(const QPointF& point) {
    if (finished) {
        return;
    }

    vertices.append(point);
    updatePolygon();
    update();
}

void BuildingItem::removeLastVertex() {
    if (!vertices.isEmpty() && !finished) {
        vertices.removeLast();
        updatePolygon();
        update();
    }
}

void BuildingItem::finishBuilding() {
    if (vertices.size() < 3) {
        qWarning() << "Cannot finish building: need at least 3 vertices";
        return;
    }

    // Calcular envolvente convexa
    vertices = computeConvexHull(vertices);

    finished = true;
    updatePolygon();
    updateAppearance();
    update();
}

void BuildingItem::clearVertices() {
    vertices.clear();
    finished = false;
    updatePolygon();
    update();
}

void BuildingItem::setVertices(const QList<QPointF>& verts) {
    vertices = verts;
    if (vertices.size() >= 3) {
        vertices = computeConvexHull(vertices);
        finished = true;
    }
    updatePolygon();
    updateAppearance();
    update();
}

void BuildingItem::updatePolygon() { prepareGeometryChange(); }

void BuildingItem::updateAppearance() {
    // Actualizar color según tipo
    update();
}

QRectF BuildingItem::boundingRect() const {
    if (vertices.isEmpty()) {
        return QRectF();
    }

    qreal minX = vertices.first().x();
    qreal minY = vertices.first().y();
    qreal maxX = minX;
    qreal maxY = minY;

    for (const QPointF& vertex : vertices) {
        minX = qMin(minX, vertex.x());
        minY = qMin(minY, vertex.y());
        maxX = qMax(maxX, vertex.x());
        maxY = qMax(maxY, vertex.y());
    }

    return QRectF(minX - 5, minY - 5, maxX - minX + 10, maxY - minY + 10);
}

QColor BuildingItem::getBuildingColor() const {
    if (buildingType == "residential") {
        return QColor(100, 149, 237, 180);  // Azul
    } else if (buildingType == "commercial") {
        return QColor(255, 165, 0, 180);  // Naranja
    } else if (buildingType == "industrial") {
        return QColor(169, 169, 169, 180);  // Gris
    } else if (buildingType == "bridge") {
        return QColor(139, 69, 19, 180);  // Marrón (Madera/Asfalto)
    } else if (buildingType == "railing") {
        return QColor(255, 0, 0, 150);  // Rojo semi-transparente
    } else if (buildingType == "overpass") {
        return QColor(128, 0, 128, 150);  // Violeta (Deck visual)
    }
    return QColor(100, 149, 237, 180);
}

void BuildingItem::paint(QPainter* painter,
                         const QStyleOptionGraphicsItem* option,
                         QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (vertices.isEmpty()) {
        return;
    }

    QColor fillColor = getBuildingColor();
    QColor lineColor = fillColor.darker(150);

    if (isSelected()) {
        lineColor = Qt::yellow;
    }

    painter->setPen(QPen(lineColor, finished ? 2 : 1));
    painter->setBrush(QBrush(fillColor));

    if (vertices.size() == 1) {
        painter->drawEllipse(vertices.first(), 5, 5);
    } else if (vertices.size() == 2) {
        painter->drawLine(vertices[0], vertices[1]);
        painter->drawEllipse(vertices[0], 3, 3);
        painter->drawEllipse(vertices[1], 3, 3);
    } else {
        QPolygonF polygon(vertices);
        painter->drawPolygon(polygon);

        if (!finished) {
            for (const QPointF& vertex : vertices) {
                painter->drawEllipse(vertex, 3, 3);
            }
        }
    }
}

QVariant BuildingItem::itemChange(GraphicsItemChange change,
                                  const QVariant& value) {
    if (change == ItemPositionChange && scene()) {
        // Podríamos validar aquí si es necesario
    }
    return QGraphicsItem::itemChange(change, value);
}

qreal BuildingItem::crossProduct(const QPointF& o, const QPointF& a,
                                 const QPointF& b) const {
    return (a.x() - o.x()) * (b.y() - o.y()) -
           (a.y() - o.y()) * (b.x() - o.x());
}

QList<QPointF> BuildingItem::computeConvexHull(
    const QList<QPointF>& points) const {
    if (points.size() < 3) {
        return points;
    }

    QList<QPointF> sortedPoints = points;

    std::sort(sortedPoints.begin(), sortedPoints.end(),
              [](const QPointF& a, const QPointF& b) {
                  return a.x() < b.x() || (a.x() == b.x() && a.y() < b.y());
              });

    QList<QPointF> lower;
    for (const QPointF& p : sortedPoints) {
        while (lower.size() >= 2 &&
               crossProduct(lower[lower.size() - 2], lower[lower.size() - 1],
                            p) <= 0) {
            lower.removeLast();
        }
        lower.append(p);
    }

    QList<QPointF> upper;
    for (int i = sortedPoints.size() - 1; i >= 0; i--) {
        const QPointF& p = sortedPoints[i];
        while (upper.size() >= 2 &&
               crossProduct(upper[upper.size() - 2], upper[upper.size() - 1],
                            p) <= 0) {
            upper.removeLast();
        }
        upper.append(p);
    }

    lower.removeLast();
    upper.removeLast();

    lower.append(upper);

    return lower;
}

BuildingVertexItem::BuildingVertexItem(int index, const QPointF& pos,
                                       QGraphicsItem* parent)
    : QGraphicsEllipseItem(-5, -5, 10, 10, parent), vertexIndex(index) {
    setPos(pos);
    setBrush(Qt::white);
    setPen(QPen(Qt::black, 1));
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

void BuildingVertexItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsEllipseItem::mousePressEvent(event);
}
