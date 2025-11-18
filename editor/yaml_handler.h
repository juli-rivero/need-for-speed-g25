#ifndef YAML_HANDLER_H
#define YAML_HANDLER_H

#include <QList>
#include <QObject>
#include <QString>

#include "building_item.h"
#include "checkpoint_item.h"
#include "hint_item.h"

class YamlHandler : public QObject {
  Q_OBJECT

 public:
  explicit YamlHandler(QObject* parent = nullptr);

  bool saveMap(const QString& filename,
               const QList<CheckpointItem*>& checkpoints,
               const QList<HintItem*>& hints,
               const QList<BuildingItem*>& buildings);

  bool loadMap(const QString& filename, QString& backgroundImagePath,
               QList<CheckpointItem*>& checkpoints, QList<HintItem*>& hints,
               QList<BuildingItem*>& buildings);
};

#endif
