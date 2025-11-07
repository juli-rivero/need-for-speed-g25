
#include "GameSessionFacade.h"
#include <thread>
#include <iostream>

GameSessionFacade::GameSessionFacade(const YamlGameConfig& configPath)
    : config(configPath) {}

void GameSessionFacade::start(const std::vector<RaceDefinition>& races,const std::vector<PlayerConfig>& players) {
    match = std::make_unique<MatchSession>(config, races, world,players);
    match->start();
    running = true;

}

void GameSessionFacade::update(float dt) {
    if (!running || !match) return;

    world.step(dt);
    // 🔹 Aplicar los estados de input actuales a cada jugador
    for (auto& [id, st] : inputStates) {
        match->applyInput(id, st);
    }
    match->update(dt);

    if (match->state() == State::Finished) {
        running = false;
    }
}
void GameSessionFacade::onPlayerEvent(PlayerId id, const std::string& event) {
    auto& st = inputStates[id];

    if (event == "accelerate_down") st.accelerate = true;
    else if (event == "accelerate_up") st.accelerate = false;
    else if (event == "brake_down") st.brake = true;
    else if (event == "brake_up") st.brake = false;
    else if (event == "turn_left_down") st.turn = "left";
    else if (event == "turn_right_down") st.turn = "right";
    else if (event == "turn_left_up" || event == "turn_right_up") st.turn = "none";
    else if (event == "nitro_toggle") st.nitro = !st.nitro;
}

WorldSnapshot GameSessionFacade::getSnapshot() const {
    return match ? match->getSnapshot() : WorldSnapshot{};
}

StaticSnapshot GameSessionFacade::getStaticSnapshot() const {
    return match ? match->getStaticSnapshot() : StaticSnapshot{};
}
void GameSessionFacade::stop() {
    running = false;
    match.reset();
}
