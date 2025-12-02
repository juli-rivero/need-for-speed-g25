#include "EntityFactory.h"

#include <memory>
#include <utility>

#include "Box2DBodyAdapter.h"
#include "Box2DPhysicsFactory.h"
#include "server/session/model/Car.h"

EntityFactory::EntityFactory(Box2DPhysicsWorld& world,
                             const YamlGameConfig& cfg)
    : world(world), cfg(cfg), physicsFactory(world.getWorldId()) {}

std::unique_ptr<Car> EntityFactory::createCar(const CarType& type,
                                              const Point& pos, float angleDeg,
                                              EntityType entType) {
    auto stats = cfg.getCarStaticStatsMap().at(type);
    auto phys = physicsFactory.createCar(pos.x, pos.y, angleDeg, stats);
    auto body = std::make_shared<Box2DBodyAdapter>(phys);
    auto car = std::make_unique<Car>(nextId(), type, stats, body, entType);

    car->setLayer(RenderLayer::UNDER);
    world.getCollisionManager().registerEntity(phys->getShapeId(), car.get());
    return car;
}

std::unique_ptr<Wall> EntityFactory::createBuilding(const Bound& bound,
                                                    EntityType type) {
    CollisionCategory cat =
        (type == EntityType::Railing) ? CATEGORY_RAILING : CATEGORY_WALL;
    const auto& [pos, w, h] = bound;

    auto phys = physicsFactory.createBuilding(pos.x, pos.y, w, h, cat);
    auto body = std::make_shared<Box2DBodyAdapter>(phys);
    auto wall = std::make_unique<Wall>(nextId(), w, h, body, type);

    world.getCollisionManager().registerEntity(phys->getShapeId(), wall.get());
    return wall;
}

std::unique_ptr<Checkpoint> EntityFactory::createCheckpoint(
    const CheckpointInfo& info) {
    const auto& [order, bound, angle, type] = info;
    const auto& [pos, w, h] = bound;
    auto phys = physicsFactory.createCheckpoint(pos.x, pos.y, w, h, angle);
    auto body = std::make_shared<Box2DBodyAdapter>(std::move(phys));
    auto cp =
        std::make_unique<Checkpoint>(nextId(), order, type, w, h, angle, body);
    world.getCollisionManager().registerEntity(body->getShapeId(), cp.get());
    return cp;
}

std::unique_ptr<BridgeSensor> EntityFactory::createBridgeSensor(
    RenderLayer type, const Bound& bound) {
    const auto& [pos, w, h] = bound;
    auto phys = physicsFactory.createBridgeSensor(pos.x, pos.y, w, h);
    auto body = std::make_shared<Box2DBodyAdapter>(phys);

    auto sensor = std::make_unique<BridgeSensor>(nextId(), type, body, w, h);

    world.getCollisionManager().registerEntity(phys->getShapeId(),
                                               sensor.get());
    return sensor;
}
std::unique_ptr<Car> EntityFactory::createNpcCar(CarType type,
                                                 const Point& pos) {
    auto stats = cfg.getCarStaticStatsMap().at(type);
    auto phys = physicsFactory.createNpcCar(pos.x, pos.y, stats);
    auto body = std::make_shared<Box2DBodyAdapter>(phys);
    auto npc =
        std::make_unique<Car>(nextId(), type, stats, body, EntityType::NPCCar);
    npc->setLayer(RenderLayer::UNDER);

    world.getCollisionManager().registerEntity(phys->getShapeId(), npc.get());
    return npc;
}
