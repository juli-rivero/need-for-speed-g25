#pragma once

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>

#include "editor/building_item.h"
#include "editor/checkpoint_item.h"
#include "editor/map_item.h"
#include "editor/sensor_item.h"  // Necesario para SensorType

class MapCanvas : public QGraphicsView {
    Q_OBJECT

   public:
    explicit MapCanvas(QWidget* parent = nullptr);
    ~MapCanvas();

    void loadBackgroundImage(const QString& imagePath);
    void clearBackgroundImage();

    // --- Checkpoints ---
    void placeCheckpoint(CheckpointItem::CheckpointType type);

    // --- Buildings ---
    void startPlacingBuilding(const QString& type);
    void finishCurrentBuilding();  // Declarado UNA sola vez
    void cancelCurrentBuilding();
    void removeLastBuildingVertex();
    void addBuildingVertex(const QPointF& pos);

    // --- Sensors (NUEVO: Lógica por vértices) ---
    void startPlacingSensor(SensorItem::SensorType type);
    void finishCurrentSensor();
    void cancelCurrentSensor();

    // --- General ---
    void addItemToScene(QGraphicsItem* item);
    void deleteSelectedItems();
    void clearAll();

    QList<CheckpointItem*> getCheckpoints() const;
    QList<BuildingItem*> getBuildings() const;
    QList<SensorItem*> getSensors() const;

    QGraphicsScene* getScene() const { return scene; }

   signals:
    void itemPlaced();
    void selectionChanged();
    void buildingVertexAdded(int count);
    void buildingFinished();
    void mousePositionChanged(const QPointF& pos);

   protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

   private:
    QGraphicsScene* scene;
    QList<MapItem*> items;

    QPixmap backgroundImage;
    QGraphicsPixmapItem* backgroundItem;

    // Estados
    bool placingCheckpoint;
    CheckpointItem::CheckpointType currentCheckpointType;

    bool placingBuilding;
    BuildingItem* currentBuilding;
    QString currentBuildingType;

    // Estado Sensores
    bool placingSensor;
    SensorItem::SensorType currentSensorType;
    SensorItem* currentSensor;  // Puntero al sensor que estamos dibujando

    void updateCursor();
};
