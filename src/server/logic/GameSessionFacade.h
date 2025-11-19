
#ifndef TALLER_TP_GAMESESSIONFACADE_H
#define TALLER_TP_GAMESESSIONFACADE_H


#include "../logic/MatchSession.h"
#include "../config/YamlGameConfig.h"
#include "../physics/PhysicsWorld.h"

class GameSessionFacade {
private:
    PhysicsWorld world;
    YamlGameConfig config;
    std::unique_ptr<MatchSession> match;
    bool running{false};

public:
    explicit GameSessionFacade(const std::string& configPath);
    void start(const std::vector<RaceDefinition>& races);
    void update(float dt);
    bool isRunning() const { return running; }
    void stop();
};

#endif
