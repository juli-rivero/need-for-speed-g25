#include "map_canvas.h"

#include <QDebug>
#include <QKeyEvent>
#include <QPainter>

#include "building_item.h"
#include "checkpoint_item.h"
#include "hint_item.h"

MapCanvas::MapCanvas(QWidget* parent)
    : QGraphicsView(parent),
      scene(new QGraphicsScene(this)),
      backgroundItem(nullptr),
      placingCheckpoint(false),
      placingHint(false),
      placingBuilding(false),
      currentCheckpointType(CheckpointItem::Start),
      currentBuilding(nullptr) {
  setScene(scene);
  setRenderHint(QPainter::Antialiasing);
  setDragMode(QGraphicsView::RubberBandDrag);

  // Configurar la escena
  scene->setSceneRect(0, 0, 1024, 768);
  setBackgroundBrush(QBrush(Qt::gray));
}

MapCanvas::~MapCanvas() { clearAll(); }

void MapCanvas::loadBackgroundImage(const QString& imagePath) {
  QPixmap pixmap(imagePath);

  if (pixmap.isNull()) {
    qWarning() << "Failed to load background image:" << imagePath;
    return;
  }

  // Limpiar imagen de fondo anterior
  clearBackgroundImage();

  // Establecer la imagen de fondo
  backgroundImage = pixmap;

  // Ajustar el tamaño de la escena al tamaño de la imagen
  scene->setSceneRect(0, 0, pixmap.width(), pixmap.height());

  // Agregar la imagen como item de fondo
  backgroundItem = scene->addPixmap(pixmap);
  backgroundItem->setZValue(-1);  // Asegurar que esté en el fondo
  backgroundItem->setFlag(QGraphicsItem::ItemIsSelectable, false);

  // Ajustar la vista
  fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

  update();
}

void MapCanvas::clearBackgroundImage() {
  if (backgroundItem) {
    scene->removeItem(backgroundItem);
    delete backgroundItem;
    backgroundItem = nullptr;
  }
  backgroundImage = QPixmap();
}

void MapCanvas::placeCheckpoint(CheckpointItem::CheckpointType type) {
  placingCheckpoint = true;
  placingHint = false;
  placingBuilding = false;
  currentCheckpointType = type;
  updateCursor();
}

void MapCanvas::placeHint() {
  placingHint = true;
  placingCheckpoint = false;
  placingBuilding = false;
  updateCursor();
}

void MapCanvas::startPlacingBuilding() {
  placingBuilding = true;
  placingCheckpoint = false;
  placingHint = false;

  if (currentBuilding) {
    finishCurrentBuilding();
  }

  currentBuilding = new BuildingItem();
  scene->addItem(currentBuilding);

  updateCursor();
}

void MapCanvas::finishCurrentBuilding() {
  if (currentBuilding && currentBuilding->getVertices().size() >= 3) {
    items.append(currentBuilding);
    currentBuilding = nullptr;
    placingBuilding = false;
    emit buildingFinished();
    emit itemPlaced();
    updateCursor();
  }
}

void MapCanvas::cancelCurrentBuilding() {
  if (currentBuilding) {
    scene->removeItem(currentBuilding);
    delete currentBuilding;
    currentBuilding = nullptr;
    placingBuilding = false;
    updateCursor();
  }
}

void MapCanvas::removeLastBuildingVertex() {
  if (currentBuilding) {
    currentBuilding->removeLastVertex();
    if (currentBuilding->getVertices().isEmpty()) {
      cancelCurrentBuilding();
    }
  }
}

void MapCanvas::addBuildingVertex(const QPointF& pos) {
  if (currentBuilding) {
    currentBuilding->addVertex(pos);
    emit buildingVertexAdded(currentBuilding->getVertices().size());
  }
}

void MapCanvas::addItemToScene(QGraphicsItem* item) {
  scene->addItem(item);
  if (MapItem* mapItem = dynamic_cast<MapItem*>(item)) {
    items.append(mapItem);
  }
}

void MapCanvas::deleteSelectedItems() {
  QList<QGraphicsItem*> selectedItems = scene->selectedItems();

  for (QGraphicsItem* item : selectedItems) {
    if (MapItem* mapItem = dynamic_cast<MapItem*>(item)) {
      items.removeAll(mapItem);
      scene->removeItem(item);
      delete item;
    }
  }

  emit selectionChanged();
}

QList<CheckpointItem*> MapCanvas::getCheckpoints() const {
  QList<CheckpointItem*> checkpoints;
  for (MapItem* item : items) {
    if (CheckpointItem* checkpoint = dynamic_cast<CheckpointItem*>(item)) {
      checkpoints.append(checkpoint);
    }
  }
  return checkpoints;
}

QList<HintItem*> MapCanvas::getHints() const {
  QList<HintItem*> hints;
  for (MapItem* item : items) {
    if (HintItem* hint = dynamic_cast<HintItem*>(item)) {
      hints.append(hint);
    }
  }
  return hints;
}

QList<BuildingItem*> MapCanvas::getBuildings() const {
  QList<BuildingItem*> buildings;
  for (MapItem* item : items) {
    if (BuildingItem* building = dynamic_cast<BuildingItem*>(item)) {
      buildings.append(building);
    }
  }
  return buildings;
}

void MapCanvas::clearAll() {
  scene->clear();
  items.clear();
  backgroundItem = nullptr;
  currentBuilding = nullptr;
  placingCheckpoint = false;
  placingHint = false;
  placingBuilding = false;
}

void MapCanvas::mousePressEvent(QMouseEvent* event) {
  QPointF scenePos = mapToScene(event->pos());

  if (event->button() == Qt::LeftButton) {
    if (placingCheckpoint) {
      static int checkpointId = 0;
      CheckpointItem* checkpoint =
          new CheckpointItem(checkpointId++, currentCheckpointType, scenePos);
      addItemToScene(checkpoint);
      placingCheckpoint = false;
      emit itemPlaced();
      updateCursor();
      return;
    }

    if (placingHint) {
      static int hintId = 0;
      HintItem* hint = new HintItem(hintId++, scenePos);
      addItemToScene(hint);
      placingHint = false;
      emit itemPlaced();
      updateCursor();
      return;
    }

    if (placingBuilding) {
      addBuildingVertex(scenePos);
      return;
    }
  }

  QGraphicsView::mousePressEvent(event);
}

void MapCanvas::mouseMoveEvent(QMouseEvent* event) {
  QPointF scenePos = mapToScene(event->pos());
  emit mousePositionChanged(scenePos);
  QGraphicsView::mouseMoveEvent(event);
}

void MapCanvas::keyPressEvent(QKeyEvent* event) {
  // Zoom con Ctrl + y Ctrl -
  if (event->modifiers() & Qt::ControlModifier) {
    if (event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal) {
      scale(1.2, 1.2);  // Zoom in
      event->accept();
      return;
    } else if (event->key() == Qt::Key_Minus) {
      scale(1.0 / 1.2, 1.0 / 1.2);  // Zoom out
      event->accept();
      return;
    } else if (event->key() == Qt::Key_0) {
      resetTransform();  // Reset zoom
      event->accept();
      return;
    }
  }

  // Manejo de otras teclas
  if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
    deleteSelectedItems();
  } else if (event->key() == Qt::Key_Escape) {
    if (placingBuilding) {
      cancelCurrentBuilding();
    } else {
      placingCheckpoint = false;
      placingHint = false;
      updateCursor();
    }
  } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
    if (placingBuilding) {
      finishCurrentBuilding();
    }
  } else if (event->key() == Qt::Key_Z &&
             (event->modifiers() & Qt::ControlModifier)) {
    if (placingBuilding) {
      removeLastBuildingVertex();
    }
  }

  QGraphicsView::keyPressEvent(event);
}

void MapCanvas::updateCursor() {
  if (placingCheckpoint || placingHint || placingBuilding) {
    setCursor(Qt::CrossCursor);
  } else {
    setCursor(Qt::ArrowCursor);
  }
}
