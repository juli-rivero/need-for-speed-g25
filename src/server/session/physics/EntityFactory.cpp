
#include "EntityFactory.h"

#include <memory>
#include <utility>

#include "Box2DBodyAdapter.h"
#include "Box2DPhysicsFactory.h"
#include "server/session/model/Car.h"
std::unique_ptr<Car> EntityFactory::createCar(Box2DPhysicsWorld& world,
                                              const CarType& type, float x,
                                              float y) {
    std::cout << "[EntityFactory] Creando auto '" << type.name << "' en (" << x
              << "," << y << ")\n";
    auto phys = std::make_shared<Box2DBodyAdapter>(
        Box2DPhysicsFactory::createCar(world.getWorldId(), x, y, type));

    auto car = std::make_unique<Car>(nextId(), type.name, type, phys);

    world.getCollisionManager().registerEntity(phys->getId(), car.get());
    std::cout << "[DEBUG] Paso 5: OK\n";
    return car;
}

std::unique_ptr<Wall> EntityFactory::createWall(Box2DPhysicsWorld& world,
                                                float x, float y, float w,
                                                float h) {
    auto phys = std::make_shared<Box2DBodyAdapter>(
        Box2DPhysicsFactory::createBuilding(world.getWorldId(), x, y, w, h));

    auto wall = std::make_unique<Wall>(nextId(), w, h, phys);
    world.getCollisionManager().registerEntity(phys->getId(), wall.get());

    return wall;
}

std::unique_ptr<Checkpoint> EntityFactory::createCheckpoint(
    Box2DPhysicsWorld& world, float x, float y, float w, float h, float angle,
    int order) {
    auto phys =
        Box2DPhysicsFactory::createCheckpoint(world.getWorldId(), x, y, w, h);
    auto body = std::make_shared<Box2DBodyAdapter>(std::move(phys));

    auto cp = std::make_unique<Checkpoint>(nextId(), order, w, h, angle, body);
    world.getCollisionManager().registerEntity(body->getId(), cp.get());
    return cp;
}

std::unique_ptr<Hint> EntityFactory::createHint(float x, float y, float angle) {
    return std::make_unique<Hint>(nextId(), x, y, angle);
}

std::unique_ptr<Bridge> EntityFactory::createBridge(Box2DPhysicsWorld& world,
                                                    float x, float y, float w,
                                                    float h, bool driveable) {
    auto lowerBody = Box2DPhysicsFactory::createBridge(world.getWorldId(), x,
                                                       y - h, w, h, false);
    auto upperBody = Box2DPhysicsFactory::createBridge(world.getWorldId(), x, y,
                                                       w, h, driveable);

    auto lower = std::make_shared<Box2DBodyAdapter>(std::move(lowerBody));
    auto upper = std::make_shared<Box2DBodyAdapter>(std::move(upperBody));

    auto bridge =
        std::make_unique<Bridge>(nextId(), w, h, lower, upper, driveable);

    world.getCollisionManager().registerEntity(lower->getId(), bridge.get());
    world.getCollisionManager().registerEntity(upper->getId(), bridge.get());

    return bridge;
}
