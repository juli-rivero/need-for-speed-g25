#ifndef MAP_CANVAS_H
#define MAP_CANVAS_H

#include <QGraphicsView>
#include <QMouseEvent>

/**
 * Canvas personalizado que captura eventos de mouse
 * y los transmite al EditorWindow para colocar items.
 */
class MapCanvas : public QGraphicsView {
    Q_OBJECT

public:
    explicit MapCanvas(QGraphicsScene* scene, QWidget* parent = nullptr);

signals:
    void canvasClicked(QPointF scenePosition);
    void canvasRightClicked(QPointF scenePosition);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    QPointF lastMousePos;
};

#endif // MAP_CANVAS_H
