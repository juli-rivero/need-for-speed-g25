#include "Box2DPhysicsWorld.h"

Box2DPhysicsWorld::Box2DPhysicsWorld() {
    b2WorldDef def = b2DefaultWorldDef();
    def.gravity = {0.0f, 0.0f};
    world = b2CreateWorld(&def);
    collisionManager = std::make_unique<CollisionManager>();
}

Box2DPhysicsWorld::~Box2DPhysicsWorld() { b2DestroyWorld(world); }

void Box2DPhysicsWorld::step(float dt) {
    b2World_Step(world, dt, 4);
    _timeAccumulator += dt;

    b2ContactEvents contactEvents = b2World_GetContactEvents(world);
    collisionManager->process(contactEvents);

    b2SensorEvents sensorEvents = b2World_GetSensorEvents(world);
    collisionManager->processSensors(sensorEvents);
}
