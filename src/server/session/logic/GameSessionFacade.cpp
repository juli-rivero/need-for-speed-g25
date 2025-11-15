
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
            if (st.leftPressed && !st.rightPressed)
                st.turn = TurnDirection::Left;
            else if (st.rightPressed && !st.leftPressed)
                st.turn = TurnDirection::Right;
            else
                st.turn = TurnDirection::None;

            match->applyInput(id, st);
        }

        world.step(dt);
        match->update(dt);
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
        st.leftPressed = true;
    else if (event == "turn_left_up")
        st.leftPressed = false;

    else if (event == "turn_right_down")
        st.rightPressed = true;
    else if (event == "turn_right_up")
        st.rightPressed = false;

    else if (event == "nitro_toggle")
        st.nitro = !st.nitro;
}
void GameSessionFacade::stop() {
    Thread::stop();
    Thread::join();
    // if (match) match->stop(); TODO: por ahora
}
