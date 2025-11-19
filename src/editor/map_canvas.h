#pragma once

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>

#include "editor/building_item.h"
#include "editor/checkpoint_item.h"
#include "editor/hint_item.h"
#include "editor/map_item.h"

class MapCanvas : public QGraphicsView {
    Q_OBJECT

   public:
    explicit MapCanvas(QWidget* parent = nullptr);
    ~MapCanvas() override;

    // Modos de colocación
    void placeCheckpoint(CheckpointItem::CheckpointType type);
    void placeHint();

    // 🆕 Edificios
    void startPlacingBuilding();
    void finishCurrentBuilding();
    void cancelCurrentBuilding();
    void removeLastBuildingVertex();

    // Selección y eliminación
    void deleteSelectedItems();

    // Add item to scene
    void addItemToScene(QGraphicsItem* item);

    // Background image
    void loadBackgroundImage(const QString& imagePath);
    void clearBackgroundImage();

    // Acceso a items
    QList<CheckpointItem*> getCheckpoints() const;
    QList<HintItem*> getHints() const;
    QList<BuildingItem*> getBuildings() const;
    QList<MapItem*> getItems() const { return items; }

    // Acceso a la escena
    QGraphicsScene* getScene() const { return scene; }

    // Limpieza
    void clearAll();

   signals:
    void selectionChanged();
    void itemPlaced();
    void buildingVertexAdded(int count);
    void buildingFinished();
    void mousePositionChanged(const QPointF& pos);

   protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

   private:
    QGraphicsScene* scene;
    QGraphicsPixmapItem* backgroundItem;  // Item para la imagen de fondo

    // Estados de colocación
    bool placingCheckpoint;
    bool placingHint;
    bool placingBuilding;
    CheckpointItem::CheckpointType currentCheckpointType;

    // Edificio en construcción
    BuildingItem* currentBuilding;

    QPixmap backgroundImage;
    QList<MapItem*> items;
    QString currentMapFile;

    void updateCursor();
    void addBuildingVertex(const QPointF& pos);
};
