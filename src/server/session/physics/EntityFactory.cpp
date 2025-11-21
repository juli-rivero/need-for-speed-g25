
#include "EntityFactory.h"

#include <memory>
#include <utility>

#include "Box2DBodyAdapter.h"
#include "Box2DPhysicsFactory.h"
#include "server/session/model/Car.h"

EntityFactory::EntityFactory(Box2DPhysicsWorld& world,
                             const YamlGameConfig& cfg)
    : world(world), cfg(cfg), physicsFactory(world.getWorldId()) {}

std::unique_ptr<Car> EntityFactory::createCar(const CarType& type, float x,
                                              float y) {
    auto stats = cfg.getCarStaticStatsMap().at(type);
    auto phys = physicsFactory.createCar(x, y, stats);
    auto body = std::make_shared<Box2DBodyAdapter>(phys);
    auto car = std::make_unique<Car>(nextId(), type, stats, body);

    world.getCollisionManager().registerEntity(phys->getShapeId(), car.get());
    return car;
}

std::unique_ptr<Wall> EntityFactory::createWall(float x, float y, float w,
                                                float h) {
    auto phys = physicsFactory.createBuilding(x, y, w, h);
    auto body = std::make_shared<Box2DBodyAdapter>(phys);
    auto wall = std::make_unique<Wall>(nextId(), w, h, body);

    world.getCollisionManager().registerEntity(phys->getShapeId(), wall.get());
    return wall;
}

std::unique_ptr<Checkpoint> EntityFactory::createCheckpoint(float x, float y,
                                                            float w, float h,
                                                            float angle,
                                                            int order) {
    auto phys = physicsFactory.createCheckpoint(x, y, w, h);
    auto body = std::make_shared<Box2DBodyAdapter>(std::move(phys));
    auto cp = std::make_unique<Checkpoint>(nextId(), order, w, h, angle, body);
    world.getCollisionManager().registerEntity(body->getShapeId(), cp.get());
    return cp;
}

std::unique_ptr<Bridge> EntityFactory::createBridge(float x, float y, float w,
                                                    float h, bool driveable) {
    auto lower = physicsFactory.createBridge(x, y - h, w, h, false);
    auto upper = physicsFactory.createBridge(x, y, w, h, driveable);

    auto lowerBody = std::make_shared<Box2DBodyAdapter>(lower);
    auto upperBody = std::make_shared<Box2DBodyAdapter>(upper);

    auto bridge = std::make_unique<Bridge>(nextId(), w, h, lowerBody, upperBody,
                                           driveable);

    world.getCollisionManager().registerEntity(lower->getShapeId(),
                                               bridge.get());
    world.getCollisionManager().registerEntity(upper->getShapeId(),
                                               bridge.get());
    return bridge;
}
// std::unique_ptr<BridgeSensor>
// EntityFactory::createBridgeSensor(BridgeSensorType type, float x, float y,
// float w,
//     float h) {
//     auto phys =
//         Box2DPhysicsFactory::createBridgeSensor(x, y, w, h);
//
//     auto body = std::make_shared<Box2DBodyAdapter>(phys);
//
//     auto sensor = std::make_unique<BridgeSensor>(nextId(), type, body);
//
//     world.getCollisionManager().registerEntity(phys->getShapeId(),
//                                                sensor.get());
//     return sensor;
// }
