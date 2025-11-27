
#include "RaceSession.h"

#include <algorithm>
#include <cmath>
#include <utility>

#include "../../config/MapLoader.h"

RaceSession::RaceSession(const YamlGameConfig& cfg,
                         std::vector<std::unique_ptr<Checkpoint>> checkpoints,
                         std::vector<SpawnPoint> spawnPoints,
                         const std::vector<Player*>& racePlayers)
    : cfg(cfg),
      players(racePlayers.begin(), racePlayers.end()),
      checkpoints(std::move(checkpoints)),
      spawnPoints(std::move(spawnPoints)) {
    orderCheckpointsByOrder();
}

void RaceSession::orderCheckpointsByOrder() {
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const std::unique_ptr<Checkpoint>& a,
                 const std::unique_ptr<Checkpoint>& b) {
                  return a->getOrder() < b->getOrder();
              });
}

void RaceSession::start() {
    _state = RaceState::Countdown;
    _raceClock = 0.0f;
    // se reinicia el estado de carrera en cada Player
    for (auto* p : players) {
        // No se setea penalidad aquí porque ya la maneja MatchSession/Player
        p->resetRaceState(/*initialPenaltyTime*/ p->getPenalty());
    }
}

void RaceSession::update(float dt) {
    if (_state == RaceState::Finished) return;

    _raceClock += dt;

    if (_state == RaceState::Countdown) {
        if (_raceClock >= countdownTime) {
            _state = RaceState::Running;
            _raceClock = 0.0f;
        }
        return;
    }

    // aca esta en running: avanza el tiempo de cada jugador
    bool everyoneDoneOrDQ = true;
    for (auto& p : players) {
        if (!p->isFinished() && !p->isDQ()) {
            p->tickTime(dt);
            everyoneDoneOrDQ = false;
        }
    }

    // Límite global de tiempo de carrera
    if (_raceClock >= cfg.getRaceTimeLimitSec()) {
        for (auto& p : players) {
            if (!p->isFinished() && !p->isDQ()) {
                p->markDQ();
            }
        }
        _state = RaceState::Finished;
        return;
    }

    if (everyoneDoneOrDQ) {
        _state = RaceState::Finished;
    }
}

void RaceSession::onCheckpointCrossed(PlayerId id, int order) const {
    for (auto* p : players) {
        PlayerSnapshot snap = p->buildSnapshot();
        if (snap.id != id) continue;
        size_t idx = p->getNextCheckpoint();
        if (idx >= checkpoints.size()) return;

        if (checkpoints[idx]->getOrder() != order) {
            return;
        }

        p->onCheckpoint(idx);

        if (p->getNextCheckpoint() >= checkpoints.size()) {
            p->markFinished();
            std::cout << "Player " << id << " terminó la carrera!" << std::endl;
        }

        return;
    }
}
void RaceSession::onCarDestroyed(PlayerId player) {
    for (auto* p : players) {
        PlayerSnapshot snap = p->buildSnapshot();
        if (snap.id == player) {
            p->markDQ();
            return;
        }
    }
}

float RaceSession::countdownRemaining() const {
    if (_state != RaceState::Countdown) return 0.f;
    return std::max(0.f, countdownTime - _raceClock);
}

std::optional<const Checkpoint*> RaceSession::nextCheckpointFor(
    PlayerId id) const {
    for (auto* p : players) {
        PlayerSnapshot snap = p->buildSnapshot();
        if (snap.id != id) continue;

        std::size_t next = p->getNextCheckpoint();
        if (next >= checkpoints.size()) return std::nullopt;

        return checkpoints[next].get();
    }
    return std::nullopt;
}

std::vector<PlayerResult> RaceSession::makeResults() const {
    std::vector<PlayerResult> out;
    out.reserve(players.size());

    for (auto* p : players) {
        PlayerSnapshot snap = p->buildSnapshot();

        PlayerResult r;
        r.id = snap.id;
        r.rawTime = p->getRawTime();
        r.penalty = p->getPenalty();
        r.netTime = p->getNetTime();
        r.dnf = p->isDQ() || !p->isFinished();

        out.push_back(r);
    }

    std::sort(
        out.begin(), out.end(),
        [](const PlayerResult& a, const PlayerResult& b) {
            if (a.dnf != b.dnf)
                return !a.dnf;  // los que NO son DNF(did not finish) primero
            return a.netTime < b.netTime;
        });
    return out;
}
