#include "PhysicsWorld.h"

PhysicsWorld::PhysicsWorld() {
    b2WorldDef def = b2DefaultWorldDef();
    def.gravity = {0.0f, 0.0f};
    world = b2CreateWorld(&def);
}

PhysicsWorld::~PhysicsWorld() {
    b2DestroyWorld(world);
}

void PhysicsWorld::step(float dt) {
    b2World_Step(world, dt, 4);
}