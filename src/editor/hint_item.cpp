#include "editor/hint_item.h"

HintItem::HintItem(int id, QPointF position, float rotation,
                   int targetCheckpoint)
    : hintId(id),
      rotation(rotation),
      targetCheckpoint(targetCheckpoint),
      dragging(false) {
    setPos(position);
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemSendsGeometryChanges);

    setZValue(5);  // Por debajo de checkpoints pero por encima del fondo

    // Almacenar datos para identificación
    setData(ItemTypeKey, HintItemType);
    setData(ItemIdKey, id);
}

QRectF HintItem::boundingRect() const {
    // Bounding box para una flecha de tamaño fijo
    float size = 40.0f;
    float margin = 10.0f;

    return QRectF(-size / 2 - margin, -size / 2 - margin, size + 2 * margin,
                  size + 2 * margin);
}

void HintItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                     QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing);

    // Guardar estado
    painter->save();

    // Aplicar rotación
    painter->rotate(rotation);

    // Dibujar círculo de fondo
    float radius = 20.0f;
    QColor bgColor(52, 152, 219);  // Azul brillante
    painter->setBrush(QBrush(bgColor));
    painter->setPen(QPen(bgColor.darker(120), 2));
    painter->drawEllipse(QPointF(0, 0), radius, radius);

    // Dibujar flecha
    QPolygonF arrow;
    arrow << QPointF(0, -12)  // Punta superior
          << QPointF(-6, 0)   // Izquierda
          << QPointF(-3, 0)   // Tallo izquierdo
          << QPointF(-3, 8)   // Base izquierda
          << QPointF(3, 8)    // Base derecha
          << QPointF(3, 0)    // Tallo derecho
          << QPointF(6, 0);   // Derecha

    painter->setBrush(QBrush(Qt::white));
    painter->setPen(QPen(Qt::white));
    painter->drawPolygon(arrow);

    // Dibujar borde de selección si está seleccionado
    if (isSelected()) {
        painter->setBrush(Qt::NoBrush);
        QPen selectionPen(Qt::yellow, 3, Qt::DashLine);
        painter->setPen(selectionPen);
        painter->drawEllipse(QPointF(0, 0), radius + 5, radius + 5);
    }

    // Dibujar número de ID pequeño en la esquina
    painter->setPen(QPen(Qt::white));
    QFont font = painter->font();
    font.setPixelSize(8);
    painter->setFont(font);
    QRectF textRect(-radius, radius - 10, 2 * radius, 10);
    painter->drawText(textRect, Qt::AlignCenter, QString::number(hintId));

    // Restaurar estado
    painter->restore();
}

void HintItem::setRotationAngle(float angle) {
    rotation = angle;
    // Normalizar entre 0-360
    while (rotation < 0) rotation += 360;
    while (rotation >= 360) rotation -= 360;
    update();
}

void HintItem::setTargetCheckpoint(int target) { targetCheckpoint = target; }

void HintItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        dragging = true;
        dragStartPos = pos();
    }
    QGraphicsItem::mousePressEvent(event);
}

void HintItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (dragging) {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void HintItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        dragging = false;
    }
    QGraphicsItem::mouseReleaseEvent(event);
}
