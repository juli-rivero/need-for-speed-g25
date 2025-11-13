
#include "EntityFactory.h"

#include <memory>
#include <utility>

#include "Box2DBodyAdapter.h"
#include "Box2DPhysicsFactory.h"
#include "server/session/model/Car.h"
std::unique_ptr<Car> EntityFactory::createCar(Box2DPhysicsWorld& world,
                                              const CarType& type,
                                              float x, float y)
{
    auto phys = Box2DPhysicsFactory::createCar(world.getWorldId(), x, y, type);
    auto body = std::make_shared<Box2DBodyAdapter>(phys);
    auto car = std::make_unique<Car>(nextId(), type.name, type, body);

    world.getCollisionManager().registerEntity(phys->getShapeId(), car.get());
    return car;
}

std::unique_ptr<Wall> EntityFactory::createWall(Box2DPhysicsWorld& world,
                                                float x, float y, float w,
                                                float h)
{
    auto phys = Box2DPhysicsFactory::createBuilding(world.getWorldId(), x, y, w, h);
    auto body = std::make_shared<Box2DBodyAdapter>(phys);
    auto wall = std::make_unique<Wall>(nextId(), w, h, body);

    world.getCollisionManager().registerEntity(phys->getShapeId(), wall.get());
    return wall;
}

std::unique_ptr<Checkpoint> EntityFactory::createCheckpoint(
    Box2DPhysicsWorld& world, float x, float y, float w, float h, float angle,
    int order)
{
    auto phys = Box2DPhysicsFactory::createCheckpoint(world.getWorldId(), x, y, w, h);
    auto body = std::make_shared<Box2DBodyAdapter>(std::move(phys));
    auto cp = std::make_unique<Checkpoint>(nextId(), order, w, h, angle, body);
    world.getCollisionManager().registerEntity(body->getShapeId(), cp.get());
    return cp;
}

std::unique_ptr<Bridge> EntityFactory::createBridge(Box2DPhysicsWorld& world,
                                                    float x, float y, float w,
                                                    float h, bool driveable)
{
    auto lower = Box2DPhysicsFactory::createBridge(world.getWorldId(), x, y - h, w, h, false);
    auto upper = Box2DPhysicsFactory::createBridge(world.getWorldId(), x, y, w, h, driveable);

    auto lowerBody = std::make_shared<Box2DBodyAdapter>(lower);
    auto upperBody = std::make_shared<Box2DBodyAdapter>(upper);

    auto bridge = std::make_unique<Bridge>(nextId(), w, h, lowerBody, upperBody, driveable);

    world.getCollisionManager().registerEntity(lower->getShapeId(), bridge.get());
    world.getCollisionManager().registerEntity(upper->getShapeId(), bridge.get());
    return bridge;
}
