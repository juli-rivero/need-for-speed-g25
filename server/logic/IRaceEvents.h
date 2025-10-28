
#ifndef TALLER_TP_IRACEEVENTS_H
#define TALLER_TP_IRACEEVENTS_H

#include "types.h"
#include "../model/Checkpoint.h"
#include "../model/Car.h"

class IRaceEvents {
public:
    virtual ~IRaceEvents() = default;
    // TODO Hooks que el subsistema de física/colisiones puede llamar
    virtual void onCheckpointCrossed(PlayerId player, int checkpointOrder) = 0;
    virtual void onCarHighImpact(PlayerId player, float impactFactor) = 0;
    virtual void onCarDestroyed(PlayerId player) = 0;
};

#endif //TALLER_TP_IRACEEVENTS_H