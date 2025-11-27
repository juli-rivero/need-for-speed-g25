#include "editor/map_canvas.h"

#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QShortcut>  // añadido

MapCanvas::MapCanvas(QWidget* parent)
    : QGraphicsView(parent),
      scene(new QGraphicsScene(this)),
      backgroundItem(nullptr),
      placingCheckpoint(false),
      currentCheckpointType(CheckpointItem::Start),
      placingBuilding(false),
      currentBuilding(nullptr),
      placingSensor(false),
      currentSensorType(SensorItem::Upper),
      currentSensor(nullptr) {
    // permitir recibir eventos de teclado
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    // Shortcuts para zoom (cubre Ctrl++ y Ctrl+= y Ctrl+-)
    auto* zoomIn1 = new QShortcut(QKeySequence("Ctrl++"), this);
    connect(zoomIn1, &QShortcut::activated, this,
            [this]() { scale(1.2, 1.2); });
    auto* zoomIn2 = new QShortcut(QKeySequence("Ctrl+="), this);
    connect(zoomIn2, &QShortcut::activated, this,
            [this]() { scale(1.2, 1.2); });
    auto* zoomOut = new QShortcut(QKeySequence("Ctrl+-"), this);
    connect(zoomOut, &QShortcut::activated, this,
            [this]() { scale(1.0 / 1.2, 1.0 / 1.2); });

    setScene(scene);
    setDragMode(QGraphicsView::RubberBandDrag);
    scene->setSceneRect(0, 0, 1024, 768);
    setBackgroundBrush(QBrush(Qt::gray));
}

MapCanvas::~MapCanvas() { clearAll(); }

// ... (loadBackgroundImage y clearBackgroundImage quedan igual) ...
void MapCanvas::loadBackgroundImage(const QString& imagePath) {
    QPixmap pixmap(imagePath);
    if (pixmap.isNull()) return;
    clearBackgroundImage();
    backgroundImage = pixmap;
    scene->setSceneRect(0, 0, pixmap.width(), pixmap.height());
    backgroundItem = scene->addPixmap(pixmap);
    backgroundItem->setZValue(-1);
    backgroundItem->setFlag(QGraphicsItem::ItemIsSelectable, false);
    fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void MapCanvas::clearBackgroundImage() {
    if (backgroundItem) {
        scene->removeItem(backgroundItem);
        delete backgroundItem;
        backgroundItem = nullptr;
    }
}

// --- CHECKPOINTS ---
void MapCanvas::placeCheckpoint(CheckpointItem::CheckpointType type) {
    placingCheckpoint = true;
    placingBuilding = false;
    placingSensor = false;
    currentCheckpointType = type;

    if (currentSensor) cancelCurrentSensor();
    if (currentBuilding) cancelCurrentBuilding();

    updateCursor();
}

// --- BUILDINGS ---
void MapCanvas::startPlacingBuilding(const QString& type) {
    placingBuilding = true;
    placingCheckpoint = false;
    placingSensor = false;
    currentBuildingType = type;

    if (currentBuilding) finishCurrentBuilding();
    if (currentSensor) cancelCurrentSensor();

    currentBuilding = new BuildingItem();
    currentBuilding->setBuildingType(type);
    scene->addItem(currentBuilding);

    updateCursor();
}

void MapCanvas::finishCurrentBuilding() {
    if (currentBuilding && currentBuilding->getVertices().size() >= 3) {
        currentBuilding
            ->finishBuilding();  // Asegurar que se cierre visualmente
        items.append(currentBuilding);
        currentBuilding = nullptr;
        // placingBuilding = false; // (Opcional: comentar para seguir
        // construyendo)
        emit buildingFinished();
        emit itemPlaced();
    } else {
        cancelCurrentBuilding();
    }
    updateCursor();
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
    }
}

void MapCanvas::addBuildingVertex(const QPointF& pos) {
    if (currentBuilding) {
        currentBuilding->addVertex(pos);
        emit buildingVertexAdded(currentBuilding->getVertices().size());
    }
}

// --- SENSORS (VÉRTICES) ---
void MapCanvas::startPlacingSensor(SensorItem::SensorType type) {
    placingSensor = true;
    placingBuilding = false;
    placingCheckpoint = false;
    currentSensorType = type;

    if (currentSensor) cancelCurrentSensor();
    if (currentBuilding) cancelCurrentBuilding();

    // Creamos el sensor vacío y empezamos a agregar vértices
    currentSensor = new SensorItem(type);
    scene->addItem(currentSensor);

    updateCursor();
}

void MapCanvas::finishCurrentSensor() {
    // Necesitamos al menos 3 vértices para un polígono
    if (currentSensor && currentSensor->getVertices().size() >= 3) {
        currentSensor->finish();
        items.append(currentSensor);
        currentSensor = nullptr;
        // placingSensor = false; // (Opcional)
        emit itemPlaced();
    } else {
        cancelCurrentSensor();
    }
    updateCursor();
}

void MapCanvas::cancelCurrentSensor() {
    if (currentSensor) {
        scene->removeItem(currentSensor);
        delete currentSensor;
        currentSensor = nullptr;
        placingSensor = false;
        updateCursor();
    }
}

// --- GENERAL ---
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

void MapCanvas::clearAll() {
    scene->clear();
    items.clear();
    backgroundItem = nullptr;
    currentBuilding = nullptr;
    currentSensor = nullptr;
    placingCheckpoint = false;
    placingBuilding = false;
    placingSensor = false;
}

QList<CheckpointItem*> MapCanvas::getCheckpoints() const {
    QList<CheckpointItem*> list;
    for (MapItem* item : items) {
        if (CheckpointItem* cp = dynamic_cast<CheckpointItem*>(item))
            list.append(cp);
    }
    return list;
}

QList<BuildingItem*> MapCanvas::getBuildings() const {
    QList<BuildingItem*> list;
    for (MapItem* item : items) {
        if (BuildingItem* b = dynamic_cast<BuildingItem*>(item)) list.append(b);
    }
    return list;
}

QList<SensorItem*> MapCanvas::getSensors() const {
    QList<SensorItem*> list;
    for (MapItem* item : items) {
        if (SensorItem* s = dynamic_cast<SensorItem*>(item)) list.append(s);
    }
    return list;
}

// --- EVENTOS ---
void MapCanvas::mousePressEvent(QMouseEvent* event) {
    QPointF scenePos = mapToScene(event->pos());

    if (event->button() == Qt::LeftButton) {
        // asegurar foco para recibir teclas luego del click
        setFocus();

        if (placingCheckpoint) {
            static int checkpointId = 0;
            CheckpointItem* checkpoint = new CheckpointItem(
                checkpointId++, currentCheckpointType, scenePos);
            addItemToScene(checkpoint);
            emit itemPlaced();
            return;
        }

        if (placingSensor) {
            if (currentSensor) {
                currentSensor->addVertex(scenePos);
            }
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
    if (event->modifiers() & Qt::ControlModifier) {
        // ... (Zoom logic igual) ...
    }

    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        deleteSelectedItems();
    } else if (event->key() == Qt::Key_Escape) {
        // CORRECCIÓN: Estructura if-else if-else limpia y con llaves
        if (placingBuilding) {
            cancelCurrentBuilding();
        } else if (placingSensor) {
            cancelCurrentSensor();
        } else {
            placingCheckpoint = false;
            updateCursor();
        }
    } else if (event->key() == Qt::Key_Return ||
               event->key() == Qt::Key_Enter) {
        if (placingBuilding) {
            finishCurrentBuilding();
        }
        if (placingSensor) {
            finishCurrentSensor();
        }
    }

    QGraphicsView::keyPressEvent(event);
}

void MapCanvas::updateCursor() {
    if (placingCheckpoint || placingBuilding || placingSensor) {
        setCursor(Qt::CrossCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
}
