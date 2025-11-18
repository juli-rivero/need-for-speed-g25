
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
        for (auto& [id, st] : inputStates) {
            match->applyInput(id, st);
        }

        world.step(dt);
        match->update(dt);
        auto& cm = world.getCollisionManager();
        if (cm.hasCollisionEvent()) {
            CollisionPacket packet;
            packet.events = cm.consumeEvents();
            // TODO(juli): MANDAR ACA
        }
        // TODO(juli)
        //  if (match->hasPendingEndRacePacket()) {
        //      auto pkt = match->consumeEndRacePacket();
        //      onRaceFinished(pkt);
        //  }

        std::this_thread::sleep_for(16ms);
    }
}

WorldSnapshot GameSessionFacade::getSnapshot() const {
    try {
        return match ? match->getSnapshot() : WorldSnapshot{};
    } catch (...) {
        std::cerr << "[ERROR] Snapshot causó crash." << std::endl;
        return WorldSnapshot{};
    }
}

StaticSnapshot GameSessionFacade::getStaticSnapshot() const {
    return match ? match->getStaticSnapshot() : StaticSnapshot{};
}

void GameSessionFacade::onPlayerEvent(PlayerId id, const std::string& event) {
    auto& st = inputStates[id];

    if (event == "accelerate_down")
        st.accelerate = true;
    else if (event == "accelerate_up")
        st.accelerate = false;

    else if (event == "brake_down")
        st.brake = true;
    else if (event == "brake_up")
        st.brake = false;
    else if (event == "turn_left_down")
        st.turn = TurnDirection::Left;
    else if (event == "turn_left_up" && st.turn == TurnDirection::Left)
        st.turn = TurnDirection::None;
    else if (event == "turn_right_down")
        st.turn = TurnDirection::Right;
    else if (event == "turn_right_up" && st.turn == TurnDirection::Right)
        st.turn = TurnDirection::None;

    else if (event == "nitro_toggle")
        st.nitro = !st.nitro;
}

// void GameSessionFacade::onRaceFinished(const EndRaceSummaryPacket& summary) {
//
// } TODO(elvis): (juli)
void GameSessionFacade::stop() {
    Thread::stop();
    Thread::join();
}
