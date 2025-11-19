#include "editor/checkpoint_item.h"

#include <cmath>

CheckpointItem::CheckpointItem(int id, CheckpointType type, QPointF position,
                               float rotation, float width)
    : checkpointId(id),
      type(type),
      rotation(rotation),
      width(width),
      dragging(false) {
    setPos(position);
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemSendsGeometryChanges);

    setZValue(10);  // Asegurar que esté por encima del fondo

    // Almacenar datos para identificación
    setData(ItemTypeKey, CheckpointItemType);
    setData(ItemIdKey, id);
    setData(CheckpointTypeKey, static_cast<int>(type));
}

QRectF CheckpointItem::boundingRect() const {
    // Rectángulo que contiene el checkpoint + margen para selección
    float height = 20.0f;  // Altura fija de la franja
    float margin = 10.0f;

    return QRectF(-width / 2 - margin, -height / 2 - margin, width + 2 * margin,
                  height + 2 * margin);
}

void CheckpointItem::paint(QPainter* painter,
                           const QStyleOptionGraphicsItem* option,
                           QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing);

    // Guardar estado
    painter->save();

    // Aplicar rotación
    painter->rotate(rotation);

    // Dibujar franja del checkpoint
    float height = 20.0f;
    QRectF rect(-width / 2, -height / 2, width, height);

    // Color según tipo
    QColor color = getColor();
    painter->setBrush(QBrush(color));
    painter->setPen(QPen(color.darker(120), 2));
    painter->drawRect(rect);

    // Dibujar borde de selección si está seleccionado
    if (isSelected()) {
        QPen selectionPen(Qt::white, 2, Qt::DashLine);
        painter->setPen(selectionPen);
        painter->setBrush(Qt::NoBrush);
        QRectF selectionRect = rect.adjusted(-5, -5, 5, 5);
        painter->drawRect(selectionRect);
    }

    // Dibujar label
    painter->setPen(QPen(Qt::black));
    QFont font = painter->font();
    font.setPixelSize(12);
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(rect, Qt::AlignCenter, getLabel());

    // Dibujar indicador de dirección (flecha pequeña)
    painter->setBrush(QBrush(Qt::black));
    QPolygonF arrow;
    arrow << QPointF(0, -height / 2 - 5) << QPointF(-5, -height / 2)
          << QPointF(5, -height / 2);
    painter->drawPolygon(arrow);

    // Restaurar estado
    painter->restore();
}

void CheckpointItem::setRotationAngle(float angle) {
    rotation = angle;
    // Normalizar entre 0-360
    while (rotation < 0) rotation += 360;
    while (rotation >= 360) rotation -= 360;
    update();
}

void CheckpointItem::setWidth(float w) {
    prepareGeometryChange();  // Notificar cambio de geometría
    width = w;
    update();
}

void CheckpointItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        dragging = true;
        dragStartPos = pos();
    }
    QGraphicsItem::mousePressEvent(event);
}

void CheckpointItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (dragging) {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void CheckpointItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        dragging = false;
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

QColor CheckpointItem::getColor() const {
    switch (type) {
        case Start:
            return QColor(46, 204, 113);  // Verde brillante
        case Normal:
            return QColor(241, 196, 15);  // Amarillo dorado
        case Finish:
            return QColor(231, 76, 60);  // Rojo brillante
        default:
            return Qt::gray;
    }
}

QString CheckpointItem::getLabel() const {
    switch (type) {
        case Start:
            return "START";
        case Finish:
            return "FINISH";
        case Normal:
            return QString("CP %1").arg(checkpointId);
        default:
            return "???";
    }
}
