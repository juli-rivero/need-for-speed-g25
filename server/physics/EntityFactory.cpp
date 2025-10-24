
#include "EntityFactory.h"
#include <memory>

#include "Box2DBodyAdapter.h"
#include "PhysicsFactory.h"

//FABRICA DE MODELOS, ENTIDADES(MODELO+FISICA)

std::unique_ptr<Car> EntityFactory::createCar(b2WorldId world,int playerId,const std::string color, float x, float y) {
    auto phys = std::make_shared<Box2DBodyAdapter>(
        std::move(*PhysicsFactory::createCar(world, x, y))
    );
    return std::make_unique<Car>(nextId(), color, phys);
}

std::unique_ptr<Wall> EntityFactory::createBuilding(b2WorldId world, float x, float y, float w, float h) {
    auto phys = std::make_shared<Box2DBodyAdapter>(
        std::move(*PhysicsFactory::createBuilding(world, x, y, w, h))
    );
    return std::make_unique<Wall>(nextId(),phys);
}

std::unique_ptr<Checkpoint> EntityFactory::createCheckpoint(b2WorldId world, float x, float y, float radius,int order) {
    auto phys = std::make_shared<Box2DBodyAdapter>(
        std::move(*PhysicsFactory::createCheckpoint(world, x, y, radius))
    );
    return std::make_unique<Checkpoint>(nextId(), order,radius , phys);
}

std::unique_ptr<Hint> EntityFactory::createHint(float x, float y,float angle) {
    return std::make_unique<Hint>(nextId(), x, y,angle);
}

std::unique_ptr<Bridge> EntityFactory::createBridge(
    b2WorldId world, float x, float y, float w, float h, bool driveable
) {
    // Se crean dos cuerpos físicos "puros"
    auto lowerBody = PhysicsFactory::createBridge(world, x, y - h, w, h, false);
    auto upperBody = PhysicsFactory::createBridge(world, x, y, w, h, driveable);

    // Envolvemos en adaptadores
    auto lower = std::make_shared<Box2DBodyAdapter>(std::move(*lowerBody));
    auto upper = std::make_shared<Box2DBodyAdapter>(std::move(*upperBody));

    // Ahora sí coincide con el constructor del modelo
    return std::make_unique<Bridge>(nextId(), lower, upper);
}