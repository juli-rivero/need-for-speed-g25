
#include "Box2DPhysicsFactory.h"

#include <iostream>
#include <memory>

#include "../../../../build/_deps/sdl2_mixer-src/src/codecs/timidity/options.h"
#include "server/session/logic/types.h"

Box2DPhysicsFactory::Box2DPhysicsFactory(const b2WorldId world)
    : world(world) {}

// --- Auto ---
std::shared_ptr<Box2dPhysicsBody> Box2DPhysicsFactory::createCar(
    float x, float y, float angleRad, const CarStaticStats& carInfo) {
    b2BodyDef def = b2DefaultBodyDef();
    def.type = b2_dynamicBody;
    def.position = {x, y};
    def.rotation = b2MakeRot(angleRad);

    def.linearDamping = carInfo.linearDamping;
    def.angularDamping = carInfo.angularDamping;

    auto body = std::make_shared<Box2dPhysicsBody>(world, def);

    b2Polygon box = b2MakeBox(carInfo.width / 2.0f, carInfo.height / 2.0f);
    b2ShapeDef sdef = b2DefaultShapeDef();
    sdef.density = carInfo.density;
    sdef.material.friction = carInfo.friction;
    sdef.material.restitution = carInfo.restitution;
    sdef.enableSensorEvents = true;
    sdef.enableContactEvents = true;
    sdef.enableHitEvents = true;

    sdef.filter.categoryBits = CATEGORY_CAR;
    sdef.filter.maskBits = 0;
    b2ShapeId shape = b2CreatePolygonShape(body->getId(), &sdef, &box);
    body->setShapeId(shape);
    return body;
}
std::shared_ptr<Box2dPhysicsBody> Box2DPhysicsFactory::createNpcCar(
    float x, float y, const CarStaticStats& carInfo) {
    b2BodyDef def = b2DefaultBodyDef();
    def.type = b2_dynamicBody;
    def.position = {x, y};
    def.linearDamping = carInfo.linearDamping;
    def.angularDamping = carInfo.angularDamping;

    auto body = std::make_shared<Box2dPhysicsBody>(world, def);

    b2Polygon box = b2MakeBox(carInfo.width / 2.0f, carInfo.height / 2.0f);

    b2ShapeDef sdef = b2DefaultShapeDef();
    sdef.density = carInfo.density;
    sdef.material.friction = carInfo.friction;
    sdef.material.restitution = carInfo.restitution;

    sdef.enableSensorEvents = true;
    sdef.enableContactEvents = true;
    sdef.enableHitEvents = true;

    sdef.filter.categoryBits = CATEGORY_NPC;
    sdef.filter.maskBits = 0;

    b2ShapeId shape = b2CreatePolygonShape(body->getId(), &sdef, &box);

    body->setShapeId(shape);
    return body;
}
// --- MURO ESTÁTICO ---
std::shared_ptr<Box2dPhysicsBody> Box2DPhysicsFactory::createBuilding(
    float x, float y, float w, float h, CollisionCategory category) {
    b2BodyDef def = b2DefaultBodyDef();
    def.type = b2_staticBody;
    def.position = {x, y};

    auto body = std::make_shared<Box2dPhysicsBody>(world, def);
    b2Polygon box = b2MakeBox(w / 2.0f, h / 2.0f);
    b2ShapeDef sdef = b2DefaultShapeDef();
    sdef.material.friction = 1.0f;
    sdef.enableContactEvents = true;
    sdef.enableHitEvents = true;

    sdef.filter.categoryBits = category;
    sdef.filter.maskBits = CATEGORY_CAR | CATEGORY_NPC;

    b2ShapeId shape = b2CreatePolygonShape(body->getId(), &sdef, &box);
    body->setShapeId(shape);
    return body;
}

// --- MURO BARANDA ---
std::shared_ptr<Box2dPhysicsBody> Box2DPhysicsFactory::createRailing(float x,
                                                                     float y,
                                                                     float w,
                                                                     float h) {
    b2BodyDef def = b2DefaultBodyDef();
    def.type = b2_staticBody;
    def.position = {x, y};

    auto body = std::make_shared<Box2dPhysicsBody>(world, def);
    b2Polygon box = b2MakeBox(w / 2.0f, h / 2.0f);
    b2ShapeDef sdef = b2DefaultShapeDef();
    sdef.material.friction = 1.0f;
    sdef.enableContactEvents = true;
    sdef.enableHitEvents = true;
    sdef.filter.categoryBits = CATEGORY_RAILING;
    sdef.filter.maskBits = CATEGORY_CAR | CATEGORY_WALL;

    b2ShapeId shape = b2CreatePolygonShape(body->getId(), &sdef, &box);
    body->setShapeId(shape);
    return body;
}

// --- CHECKPOINT (sensor) ---
std::shared_ptr<Box2dPhysicsBody> Box2DPhysicsFactory::createCheckpoint(
    float x, float y, float w, float h, float angleRad) {
    b2BodyDef def = b2DefaultBodyDef();
    def.type = b2_staticBody;
    def.position = {x, y};
    def.rotation = b2MakeRot(angleRad);

    auto body = std::make_shared<Box2dPhysicsBody>(world, def);
    b2Polygon box = b2MakeBox(w / 2.0f, h / 2.0f);
    b2ShapeDef sdef = b2DefaultShapeDef();
    sdef.isSensor = true;
    sdef.enableSensorEvents = true;
    sdef.filter.categoryBits = CATEGORY_SENSOR;
    sdef.filter.maskBits = CATEGORY_CAR;

    b2ShapeId shape = b2CreatePolygonShape(body->getId(), &sdef, &box);
    body->setShapeId(shape);
    return body;
}

// --- BridgeSensor ---
std::shared_ptr<Box2dPhysicsBody> Box2DPhysicsFactory::createBridgeSensor(
    float x, float y, float w, float h) {
    b2BodyDef def = b2DefaultBodyDef();
    def.type = b2_staticBody;
    def.position = {x, y};

    auto body = std::make_shared<Box2dPhysicsBody>(world, def);
    b2Polygon box = b2MakeBox(w / 2.0f, h / 2.0f);
    b2ShapeDef sdef = b2DefaultShapeDef();
    sdef.isSensor = true;
    sdef.enableSensorEvents = true;

    sdef.filter.categoryBits = CATEGORY_SENSOR;
    sdef.filter.maskBits = CATEGORY_CAR | CATEGORY_NPC;

    b2ShapeId sh = b2CreatePolygonShape(body->getId(), &sdef, &box);
    body->setShapeId(sh);
    return body;
}
