#pragma once

#include <QList>
#include <QObject>
#include <QString>

#include "editor/building_item.h"
#include "editor/checkpoint_item.h"
#include "editor/hint_item.h"

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
