#pragma once

#include <QList>
#include <QObject>
#include <QString>

class CheckpointItem;
class BuildingItem;
class SensorItem;

class YamlHandler : public QObject {
    Q_OBJECT

   public:
    explicit YamlHandler(QObject* parent = nullptr);

    bool saveMap(const QString& filename,
                 const QList<CheckpointItem*>& checkpoints,
                 const QList<BuildingItem*>& buildings,
                 const QList<SensorItem*>& sensors);

    bool loadMap(const QString& filename, QString& backgroundImagePath,
                 QList<CheckpointItem*>& checkpoints,
                 QList<BuildingItem*>& buildings, QList<SensorItem*>& sensors);
};
