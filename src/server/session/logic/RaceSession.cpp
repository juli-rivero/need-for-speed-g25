
#include "RaceSession.h"

#include <algorithm>
#include <cmath>
#include <utility>

#include "../../config/MapLoader.h"

RaceSession::RaceSession(
    const YamlGameConfig& cfg, CityId city,
    std::vector<std::unique_ptr<Checkpoint>> checkpoints,
    const std::vector<std::shared_ptr<Car>>& playersCars,
    const std::vector<PlayerId>& playerIds,
    std::vector<SpawnPoint> spawn_points,
    std::unordered_map<PlayerId, float> initialPenaltiesForThisRace)
    : _cfg(cfg),
      _city(city),
      _checkpoints(std::move(checkpoints)),
      _spawnPoints(std::move(spawn_points)) {
    _players.reserve(playersCars.size());
    for (size_t i = 0; i < playersCars.size(); ++i) {
        PlayerRaceData prd;
        prd.id = playerIds[i];
        prd.car = playersCars[i];
        prd.penaltyTime = initialPenaltiesForThisRace[prd.id];
        _players.push_back(std::move(prd));
    }
    orderCheckpointsByOrder();
}

void RaceSession::orderCheckpointsByOrder() {
    std::sort(_checkpoints.begin(), _checkpoints.end(),
              [](const std::unique_ptr<Checkpoint>& a,
                 const std::unique_ptr<Checkpoint>& b) {
                  return a->getOrder() < b->getOrder();
              });
}

void RaceSession::start() {
    _state = RaceState::Countdown;
    _raceClock = 0.0f;
}

bool RaceSession::everyoneDoneOrDQ() const {
    for (const auto& p : _players) {
        if (!p.finished && !p.disqualified) return false;
    }
    return true;
}

void RaceSession::applyTimeLimitIfNeeded() {
    if (_raceClock >= _cfg.getRaceTimeLimitSec()) {
        // tiempo agotado marcar DNF a quienes no terminaron
        for (auto& p : _players) {
            if (!p.finished && !p.disqualified) {
                p.disqualified = true;  // DNF por timeout
            }
        }
        _state = RaceState::Finished;
    }
}

void RaceSession::update(float dt) {
    if (_state == RaceState::Finished) return;

    _raceClock += dt;

    if (_state == RaceState::Countdown) {
        if (_raceClock >= _countdownTime) {
            _state = RaceState::Running;
            // reinicia reloj de carrera efectiva
            _raceClock = 0.0f;
        }
        return;
    }

    // Running
    for (auto& p : _players) {
        if (!p.finished && !p.disqualified) {
            p.elapsed += dt;  // acumula tiempo crudo
            // Llegó a meta?
            if (p.nextCheckpoint >= _checkpoints.size()) {
                p.finished = true;
            }
        }
    }

    if (everyoneDoneOrDQ()) {
        _state = RaceState::Finished;
        return;
    }

    applyTimeLimitIfNeeded();
}

std::vector<PlayerResult> RaceSession::makeResults() const {
    std::vector<PlayerResult> out;
    out.reserve(_players.size());
    for (const auto& p : _players) {
        PlayerResult r;
        r.id = p.id;
        r.penalty = p.penaltyTime;
        r.netTime = p.finished ? (p.elapsed + p.penaltyTime) : p.elapsed;
        r.dnf = (!p.finished || p.disqualified);
        out.push_back(r);
    }

    std::sort(out.begin(), out.end(),
              [](const PlayerResult& a, const PlayerResult& b) {
                  // Los que completaron la carrera primero, luego por tiempo
                  if (a.dnf != b.dnf) {
                      return !a.dnf;  // false (no DNF) < true (DNF)
                  }
                  return a.netTime < b.netTime;
              });
    return out;
}

// Visibilidad: siguiente CP e hints asociados
std::optional<const Checkpoint*> RaceSession::nextCheckpointFor(
    PlayerId p) const {
    auto it = std::find_if(_players.begin(), _players.end(),
                           [&](const auto& pr) { return pr.id == p; });
    if (it == _players.end()) return std::nullopt;
    if (it->nextCheckpoint >= _checkpoints.size()) return std::nullopt;
    return _checkpoints[it->nextCheckpoint].get();
}

// IRaceEvents desde colisiones/sensores
void RaceSession::onCheckpointCrossed(PlayerId player, int checkpointOrder) {
    auto itP = std::find_if(_players.begin(), _players.end(),
                            [&](const auto& pr) { return pr.id == player; });
    if (itP == _players.end() || itP->finished || itP->disqualified) return;

    if (itP->nextCheckpoint < _checkpoints.size() &&
        _checkpoints[itP->nextCheckpoint]->getOrder() == checkpointOrder) {
        ++(itP->nextCheckpoint);
    }
    if (itP->nextCheckpoint >= _checkpoints.size()) {
        itP->finished = true;
        std::cout << " Player " << player << " completó la carrera!\n";
    }
}
RaceProgressSnapshot RaceSession::getProgressForPlayer(PlayerId id) const {
    RaceProgressSnapshot rp{};
    auto it = std::find_if(_players.begin(), _players.end(),
                           [&](const auto& p) { return p.id == id; });
    if (it != _players.end()) {
        rp.playerId = it->id;
        rp.nextCheckpoint = it->nextCheckpoint;
        rp.finished = it->finished;
        rp.disqualified = it->disqualified;
        rp.elapsedTime = it->elapsed;
    }
    return rp;
}

void RaceSession::onCarDestroyed(PlayerId player) {
    auto itP = std::find_if(_players.begin(), _players.end(),
                            [&](const auto& pr) { return pr.id == player; });
    if (itP == _players.end()) return;
    itP->disqualified = true;
}
