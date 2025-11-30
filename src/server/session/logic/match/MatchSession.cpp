
#include "MatchSession.h"

#include <utility>

MatchSession::MatchSession(const YamlGameConfig& cfg,
                           std::vector<std::string> raceFiles,
                           Box2DPhysicsWorld& world,
                           std::vector<PlayerConfig> playersCfg)
    : _cfg(cfg),
      _world(world),
      _raceFiles(std::move(raceFiles)),
      _playerConfigs(std::move(playersCfg)) {
    if (_raceFiles.empty()) {
        _state = MatchState::Finished;
        return;
    }

    _state = MatchState::Racing;
    startRace(0);
}

void MatchSession::update(float dt) {
    switch (_state) {
        case MatchState::Starting:
        case MatchState::Racing:
            for (auto& [id, player] : _players) {
                player->update(dt);
            }
            if (_traffic) _traffic->update(dt);

            _race->update(dt);

            if (_race->isFinished()) {
                startIntermission();
            }
            break;

        case MatchState::Intermission:
            _intermissionClock += dt;
            if (_intermissionClock >= _cfg.getIntermissionSec()) {
                endIntermissionAndPrepareNextRace();
            }
            break;

        case MatchState::Finished:
            break;
    }
}

void MatchSession::applyInput(PlayerId id, const CarInput& input) const {
    if (_race && _race->getState() != RaceState::Countdown)
        _players.at(id)->applyInput(input);
}
