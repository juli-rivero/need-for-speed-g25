
#include "GameSessionFacade.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using std::chrono_literals::operator""ms;
GameSessionFacade::GameSessionFacade(const YamlGameConfig& configPath)
    : config(configPath) {}

void GameSessionFacade::start(const std::vector<RaceDefinition>& races,
                              const std::vector<PlayerConfig>& players) {
    match = std::make_unique<MatchSession>(config, races, world, players);
    match->start();
    Thread::start();
}
void GameSessionFacade::run() {
    const float dt = 1.f / 60.f;

    while (should_keep_running()) {
        std::pair<PlayerId, CarInput> input;
        while (queue_actions.try_pop(input)) {
            match->applyInput(input.first, input.second);
        }

        world.step(dt);
        match->update(dt);

        emitter.dispatch(&Listener::on_snapshot, match->getSnapshot());
        std::this_thread::sleep_for(16ms);
    }
}

void GameSessionFacade::stop() {
    Thread::stop();
    Thread::join();
    // if (match) match->stop(); TODO: por ahora
}

void GameSessionFacade::Listener::subscribe(GameSessionFacade& g) {
    common::Listener<GameSessionFacade::Listener>::subscribe(g.emitter);
}
void GameSessionFacade::startTurningLeft(PlayerId id) {
    queue_actions.try_push({id, CarInput::StartTurningLeft});
}
void GameSessionFacade::stopTurningLeft(PlayerId id) {
    queue_actions.try_push({id, CarInput::StopTurningLeft});
}
void GameSessionFacade::startTurningRight(PlayerId id) {
    queue_actions.try_push({id, CarInput::StartTurningRight});
}
void GameSessionFacade::stopTurningRight(PlayerId id) {
    queue_actions.try_push({id, CarInput::StopTurningRight});
}
void GameSessionFacade::startAccelerating(PlayerId id) {
    queue_actions.try_push({id, CarInput::StartAccelerating});
}
void GameSessionFacade::stopAccelerating(PlayerId id) {
    queue_actions.try_push({id, CarInput::StopAccelerating});
}
void GameSessionFacade::startReversing(PlayerId id) {
    queue_actions.try_push({id, CarInput::StartReversing});
}
void GameSessionFacade::stopReversing(PlayerId id) {
    queue_actions.try_push({id, CarInput::StopReversing});
}
void GameSessionFacade::useNitro(PlayerId id) {
    queue_actions.try_push({id, CarInput::StartUsingNitro});
}
