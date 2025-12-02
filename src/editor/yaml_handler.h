#pragma once

#include <QList>
#include <QObject>
#include "editor/building_item.h"
#include "editor/checkpoint_item.h"
#include "editor/overpass_item.h"
#include "editor/sensor_item.h"

class YamlHandler : public QObject {
    Q_OBJECT
   public:
    explicit YamlHandler(QObject* parent = nullptr);

    bool saveMap(const QString& filename,
                 const QList<CheckpointItem*>& checkpoints,
                 const QList<BuildingItem*>& buildings,
                 const QList<SensorItem*>& sensors,
                 const QList<OverpassItem*>& overpasses);

    bool loadMap(const QString& filename, QString& backgroundImagePath,
                 QList<CheckpointItem*>& checkpoints,
                 QList<BuildingItem*>& buildings, QList<SensorItem*>& sensors,
                 QList<OverpassItem*>& overpasses);
};
