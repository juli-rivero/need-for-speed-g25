
#include "GameSessionFacade.h"

#include <chrono>
#include <vector>

#include "common/timer_iterator.h"

using std::chrono_literals::operator""ms;

GameSessionFacade::GameSessionFacade(const YamlGameConfig& configPath,
                                     const std::vector<std::string>& raceFiles,
                                     const std::vector<PlayerConfig>& players)
    : world(), match(configPath, raceFiles, world, players) {}

void GameSessionFacade::run() {
    constexpr std::chrono::duration<double> dt(1.f / 60.f);

    TimerIterator iterator(dt);

    size_t passed_iterations = 0;

    while (should_keep_running()) {
        std::pair<PlayerId, CarInput> input;
        while (queue_actions.try_pop(input)) {
            match.applyInput(input.first, input.second);
        }

        world.step(passed_iterations * dt.count());
        match.update(passed_iterations * dt.count());

        emitter.dispatch(&Listener::on_snapshot, match.getSnapshot());

        auto& collisionManager = world.getCollisionManager();
        while (collisionManager.hasCollisionEvent()) {
            const CollisionEvent& collision = collisionManager.consumeEvent();
            emitter.dispatch(&Listener::on_collision_event, collision);
        }

        // TODO(juli)
        // if (match.hasFinalSummary()) {
        //     auto pkt = match.consumeFinalSummary();
        //     emitter.dispatch(&Listener::on_match_finished, pkt);
        // }

        passed_iterations = iterator.next();
    }
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
