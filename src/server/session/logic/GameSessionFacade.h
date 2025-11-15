#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../config/YamlGameConfig.h"
#include "../logic/MatchSession.h"
#include "../physics/Box2DPhysicsWorld.h"
#include "common/thread.h"

class GameSessionFacade : public Thread {
   private:
    Box2DPhysicsWorld world;
    const YamlGameConfig& config;
    std::unique_ptr<MatchSession> match;
    std::unordered_map<PlayerId, PlayerInput> inputStates;
    using Thread::start;

   public:
    explicit GameSessionFacade(const YamlGameConfig& configPath);

    void start(const std::vector<RaceDefinition>& races,
               const std::vector<PlayerConfig>& players);

    void run() override;

    void onPlayerEvent(PlayerId id, const std::string& event);

    WorldSnapshot getSnapshot() const;

    StaticSnapshot getStaticSnapshot() const;

    void stop() override;
};
