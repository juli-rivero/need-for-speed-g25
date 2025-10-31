
#ifndef TALLER_TP_GAMESESSIONFACADE_H
#define TALLER_TP_GAMESESSIONFACADE_H


#include "../logic/MatchSession.h"
#include "../config/YamlGameConfig.h"
#include "../physics/Box2DPhysicsWorld.h"

class GameSessionFacade {
private:
    Box2DPhysicsWorld world;
    YamlGameConfig config;
    std::unique_ptr<MatchSession> match;
    bool running{false};

public:
    explicit GameSessionFacade(const std::string& configPath);//TODO: talvez sea mejor que reciba una referencia a yamlgameconfig, de esa forma se instancia una sola vez antes y no aca
    void start(const std::vector<RaceDefinition>& races);
    void update(float dt);
    bool isRunning() const { return running; }
    void stop();
};

#endif
