
#include "Box2DPhysicsFactory.h"

#include <iostream>
#include <memory>

// --- Auto ---
std::shared_ptr<Box2dPhysicsBody> Box2DPhysicsFactory::createCar(
    b2WorldId world, float x, float y, const CarType& type)
{
    b2BodyDef def = b2DefaultBodyDef();
    def.type = b2_dynamicBody;
    def.position = {x, y};
    def.linearDamping = type.linearDamping;
    def.angularDamping = type.angularDamping;

    auto body = std::make_shared<Box2dPhysicsBody>(world, def);

    b2Polygon box = b2MakeBox(type.width / 2.0f, type.height / 2.0f);
    b2ShapeDef sdef = b2DefaultShapeDef();
    sdef.density = type.density;
    sdef.material.friction = type.friction;
    sdef.material.restitution = type.restitution;
    sdef.enableSensorEvents = true;
    sdef.enableContactEvents = true;
    sdef.enableHitEvents = true;

    b2ShapeId shape = b2CreatePolygonShape(body->getId(), &sdef, &box);
    body->setShapeId(shape);
    return body;
}

// --- MURO ESTÁTICO ---
std::shared_ptr<Box2dPhysicsBody> Box2DPhysicsFactory::createBuilding(
    b2WorldId world, float x, float y, float w, float h)
{
    b2BodyDef def = b2DefaultBodyDef();
    def.type = b2_staticBody;
    def.position = {x, y};

    auto body = std::make_shared<Box2dPhysicsBody>(world, def);
    b2Polygon box = b2MakeBox(w / 2.0f, h / 2.0f);
    b2ShapeDef sdef = b2DefaultShapeDef();
    sdef.material.friction = 1.0f;
    sdef.enableContactEvents = true;
    sdef.enableHitEvents = true;

    b2ShapeId shape = b2CreatePolygonShape(body->getId(), &sdef, &box);
    body->setShapeId(shape);
    return body;
}

// --- Checkpoint  ---
// --- CHECKPOINT (sensor) ---
std::shared_ptr<Box2dPhysicsBody> Box2DPhysicsFactory::createCheckpoint(
    b2WorldId world, float x, float y, float w, float h)
{
    b2BodyDef def = b2DefaultBodyDef();
    def.type = b2_staticBody;
    def.position = {x, y};

    auto body = std::make_shared<Box2dPhysicsBody>(world, def);
    b2Polygon box = b2MakeBox(w / 2.0f, h / 2.0f);
    b2ShapeDef sdef = b2DefaultShapeDef();
    sdef.isSensor = true;
    sdef.enableSensorEvents = true;

    b2ShapeId shape = b2CreatePolygonShape(body->getId(), &sdef, &box);
    body->setShapeId(shape);
    return body;
}


// --- PUENTE (doble cuerpo, arriba/abajo) ---
std::shared_ptr<Box2dPhysicsBody> Box2DPhysicsFactory::createBridge(
    b2WorldId world, float x, float y, float w, float h, bool driveable)
{
    b2BodyDef def = b2DefaultBodyDef();
    def.type = b2_staticBody;
    def.position = {x, y};

    auto body = std::make_shared<Box2dPhysicsBody>(world, def);
    b2Polygon box = b2MakeBox(w / 2.0f, h / 2.0f);
    b2ShapeDef sdef = b2DefaultShapeDef();
    sdef.material.friction = driveable ? 0.7f : 1.0f;
    sdef.isSensor = !driveable;

    b2ShapeId shape = b2CreatePolygonShape(body->getId(), &sdef, &box);
    body->setShapeId(shape);
    return body;
}
