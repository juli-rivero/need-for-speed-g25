

#include "RaceSession.h"
#include <algorithm>
#include <cmath>

RaceSession::RaceSession(const YamlGameConfig& cfg,
                         CityId city,
                         std::vector<Checkpoint> checkpoints,
                         std::vector<Hint> hints,
                         std::vector<std::shared_ptr<Car>> playersCars,
                         std::unordered_map<PlayerId, float> initialPenaltiesForThisRace)
    : _cfg(cfg), _city(city), _checkpoints(std::move(checkpoints)), _hints(std::move(hints)) {

    orderCheckpointsByOrder();

    _players.reserve(playersCars.size());
    for (const auto& carPtr : playersCars) {
        PlayerRaceData prd;
        prd.id = static_cast<PlayerId>(carPtr->getId());
        prd.car = carPtr;
        prd.penaltyTime = initialPenaltiesForThisRace[prd.id];
        _players.push_back(std::move(prd));
    }
}

void RaceSession::orderCheckpointsByOrder() {
    std::sort(_checkpoints.begin(), _checkpoints.end(),
              [](const Checkpoint& a, const Checkpoint& b){
                  return a.getOrder() < b.getOrder();
              });
}

void RaceSession::start() { _state = State::Countdown; _raceClock = 0.0f; }

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
                p.disqualified = true; // DNF por timeout
            }
        }
        _state = State::Finished;
    }
}

void RaceSession::update(float dt) {
    if (_state == State::Finished) return;

    _raceClock += dt;

    if (_state == State::Countdown) {
        if (_raceClock >= _countdownTime) {
            _state = State::Running;
            // reinicia reloj de carrera efectiva
            _raceClock = 0.0f;
        }
        return;
    }

    // Running
    for (auto& p : _players) {
        if (!p.finished && !p.disqualified) {
            p.elapsed += dt; // acumula tiempo crudo
            // Llegó a meta?
            if (p.nextCheckpoint >= _checkpoints.size()) {
                p.finished = true;
            }
        }
    }

    if (everyoneDoneOrDQ()) {
        _state = State::Finished;
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
        r.netTime = p.finished ? (p.elapsed - p.penaltyTime) : p.elapsed;
        r.dnf = (!p.finished || p.disqualified);
        out.push_back(r);
    }

    std::sort(out.begin(), out.end(), [](const PlayerResult& a, const PlayerResult& b){
        if (a.dnf != b.dnf) return !a.dnf && b.dnf;
        return a.netTime < b.netTime;
    });
    return out;
}

// Visibilidad: siguiente CP e hints asociados
std::optional<Checkpoint> RaceSession::nextCheckpointFor(PlayerId p) const {
    auto it = std::find_if(_players.begin(), _players.end(),
                           [&](const auto& pr){ return pr.id == p; });
    if (it == _players.end()) return std::nullopt;
    if (it->nextCheckpoint >= _checkpoints.size()) return std::nullopt;
    return _checkpoints[it->nextCheckpoint];
}

std::vector<Hint> RaceSession::hintsTowardsNextFor(PlayerId p) const {
    // Todo: una estrategia simple para devolver todos los hints del mapa o filtrar por "sector".
    (void)p; // evita warning mientras no se usa
    return _hints;
}

// IRaceEvents desde colisiones/sensores
void RaceSession::onCheckpointCrossed(PlayerId player, int checkpointOrder) {
    auto itP = std::find_if(_players.begin(), _players.end(),
                            [&](const auto& pr){ return pr.id == player; });
    if (itP == _players.end() || itP->finished || itP->disqualified) return;


    if (itP->nextCheckpoint < _checkpoints.size() &&
        _checkpoints[itP->nextCheckpoint].getOrder() == checkpointOrder) {
        ++(itP->nextCheckpoint);
    }
}

void RaceSession::onCarHighImpact(PlayerId player, float impactFactor) {
    // ejemplo: daño y reducción de velocidad (TODO: delegar al Car)
    auto itP = std::find_if(_players.begin(), _players.end(),
                            [&](const auto& pr){ return pr.id == player; });
    if (itP == _players.end() || itP->finished || itP->disqualified) return;

    if (auto car = itP->car) {
        //TODO car->applyDamage(impactFactor * cfgFactor); // agrega a tu Car
        // si cae a 0 → destruir
        // if (car->health() <= 0) onCarDestroyed(player);
        float dmg = impactFactor * 0.5f;
        car->damage(dmg);
        if (car->isDestroyed()) {
            onCarDestroyed(player);
        }
    }
}

void RaceSession::onCarDestroyed(PlayerId player) {
    auto itP = std::find_if(_players.begin(), _players.end(),
                            [&](const auto& pr){ return pr.id == player; });
    if (itP == _players.end()) return;
    itP->disqualified = true;
}
