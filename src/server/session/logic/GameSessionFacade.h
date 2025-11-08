#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../config/YamlGameConfig.h"
#include "../logic/MatchSession.h"
#include "../physics/Box2DPhysicsWorld.h"

class GameSessionFacade {
   private:
    Box2DPhysicsWorld world;
    const YamlGameConfig& config;
    std::unique_ptr<MatchSession> match;
    bool running{false};
    // estados persistentes de input por jugador
    std::unordered_map<PlayerId, PlayerInput> inputStates;

   public:
    explicit GameSessionFacade(const YamlGameConfig& configPath);
    void start(const std::vector<RaceDefinition>& races,
               const std::vector<PlayerConfig>& players);
    void update(float dt);
    bool isRunning() const { return running; }
    void stop();
    // para manejar eventos de teclado
    void onPlayerEvent(PlayerId id, const std::string& event);

    WorldSnapshot getSnapshot() const;
    StaticSnapshot getStaticSnapshot() const;
};
