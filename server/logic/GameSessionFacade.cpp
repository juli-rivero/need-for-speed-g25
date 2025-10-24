
#include "GameSessionFacade.h"
#include <thread>
#include <iostream>

GameSessionFacade::GameSessionFacade(const std::string& configPath)
    : config(configPath) {}

void GameSessionFacade::start(const std::vector<RaceDefinition>& races) {
    match = std::make_unique<MatchSession>(config, races, world);
    match->start();
    running = true;

}

void GameSessionFacade::update(float dt) {
    if (!running || !match) return;

    world.step(dt);
    match->update(dt);

    if (match->state() == MatchSession::State::Finished) {
        running = false;
    }
}

void GameSessionFacade::stop() {
    running = false;
    match.reset();
}
