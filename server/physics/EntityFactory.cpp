
#include "EntityFactory.h"

#include "Box2DBodyAdapter.h"
#include "Box2DPhysicsFactory.h"

// 🚗 Car
std::unique_ptr<Car> EntityFactory::createCar(Box2DPhysicsWorld& world, const std::string& color, float x, float y) {
    auto phys = std::make_shared<Box2DBodyAdapter>(
        Box2DPhysicsFactory::createCar(world.getWorldId(), x, y)
    );

    auto car = std::make_unique<Car>(nextId(), color, phys);

    // 🔥 registrar en el CollisionManager
    world.getCollisionManager().registerEntity(phys->getId(), car.get());

    return car;
}

// 🧱 Wall
std::unique_ptr<Wall> EntityFactory::createWall(Box2DPhysicsWorld& world, float x, float y, float w, float h) {
    auto phys = std::make_shared<Box2DBodyAdapter>(
        Box2DPhysicsFactory::createBuilding(world.getWorldId(), x, y, w, h)
    );

    auto wall = std::make_unique<Wall>(nextId(), phys);
    world.getCollisionManager().registerEntity(phys->getId(), wall.get());
    return wall;
}

// 🚩 Checkpoint
std::unique_ptr<Checkpoint> EntityFactory::createCheckpoint(Box2DPhysicsWorld& world, float x, float y, float w, float h, int order) {
    auto phys = Box2DPhysicsFactory::createCheckpoint(world.getWorldId(), x, y, w,h);
    auto body = std::make_shared<Box2DBodyAdapter>(std::move(phys));

    auto cp = std::make_unique<Checkpoint>(nextId(), order,w,h, body);
    world.getCollisionManager().registerEntity(body->getId(), cp.get());
    return cp;
}

// ➡️ Hint
std::unique_ptr<Hint> EntityFactory::createHint(float x, float y, float angle) {
    return std::make_unique<Hint>(nextId(), x, y, angle);
}

// 🌉 Bridge
std::unique_ptr<Bridge> EntityFactory::createBridge(Box2DPhysicsWorld& world, float x, float y, float w, float h, bool driveable) {
    auto lowerBody = Box2DPhysicsFactory::createBridge(world.getWorldId(), x, y - h, w, h, false);
    auto upperBody = Box2DPhysicsFactory::createBridge(world.getWorldId(), x, y, w, h, driveable);

    auto lower = std::make_shared<Box2DBodyAdapter>(std::move(lowerBody));
    auto upper = std::make_shared<Box2DBodyAdapter>(std::move(upperBody));

    auto bridge = std::make_unique<Bridge>(nextId(), lower, upper);

    world.getCollisionManager().registerEntity(lower->getId(), bridge.get());
    world.getCollisionManager().registerEntity(upper->getId(), bridge.get());

    return bridge;
}