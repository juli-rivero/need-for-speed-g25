#include "map_canvas.h"

MapCanvas::MapCanvas(QGraphicsScene* scene, QWidget* parent)
    : QGraphicsView(scene, parent) {
    
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

void MapCanvas::mousePressEvent(QMouseEvent* event) {
    QPointF scenePos = mapToScene(event->pos());
    lastMousePos = scenePos;
    
    // Si se hace click izquierdo en un área vacía (sin items)
    QGraphicsItem* item = itemAt(event->pos());
    
    if (event->button() == Qt::LeftButton) {
        if (!item) {
            // Click en área vacía -> emitir señal para colocar item
            emit canvasClicked(scenePos);
        } else {
            // Click en un item existente -> dejar que Qt lo maneje (selección)
            QGraphicsView::mousePressEvent(event);
        }
    } else if (event->button() == Qt::RightButton) {
        emit canvasRightClicked(scenePos);
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

void MapCanvas::mouseMoveEvent(QMouseEvent* event) {
    lastMousePos = mapToScene(event->pos());
    QGraphicsView::mouseMoveEvent(event);
}
