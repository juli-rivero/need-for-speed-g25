#include "RaceSession.h"

void RaceSession::orderCheckpointsByOrder() {
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](auto& a, auto& b) { return a->getOrder() < b->getOrder(); });
}

void RaceSession::onCheckpointCrossed(PlayerId id, int order) const {
    for (auto* p : players) {
        if (p->getId() != id) continue;

        size_t idx = p->getNextCheckpoint();
        if (idx >= checkpoints.size()) return;

        if (checkpoints[idx]->getOrder() != order) return;

        p->onCheckpoint(idx);

        if (p->getNextCheckpoint() >= checkpoints.size()) {
            p->markFinished();
            spdlog::info("Player {} gano la carrera", p->getId());
        }
        return;
    }
}

void RaceSession::onCarDestroyed(PlayerId id) {
    for (auto* p : players)
        if (p->getId() == id) p->markDQ();
}

std::optional<const Checkpoint*> RaceSession::nextCheckpointFor(
    PlayerId id) const {
    for (auto* p : players) {
        if (p->getId() != id) continue;

        auto idx = p->getNextCheckpoint();
        if (idx >= checkpoints.size()) return std::nullopt;

        return checkpoints[idx].get();
    }
    return std::nullopt;
}
