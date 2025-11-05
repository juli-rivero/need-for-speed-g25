
#include <memory>
#include "Box2DPhysicsFactory.h"

// --- Auto ---
std::unique_ptr<Box2dPhysicsBody> Box2DPhysicsFactory::createCar(b2WorldId world, float x, float y,CarType type) {
    b2BodyDef def = b2DefaultBodyDef();
    def.type = b2_dynamicBody;
    def.position = {x, y};
    def.linearDamping = type.linearDamping;
    def.angularDamping = type.angularDamping;

    auto body = std::make_unique<Box2dPhysicsBody>(world, def);

    b2Polygon box = b2MakeBox(1.0f, 2.0f);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = type.density;
    shapeDef.material.friction = type.friction;
    shapeDef.material.restitution = type.restitution;

    b2CreatePolygonShape(body->getId(), &shapeDef, &box);
    return body;
}

// --- Edificio estático, no se mueve ---
std::unique_ptr<Box2dPhysicsBody> Box2DPhysicsFactory::createBuilding(b2WorldId world, float x, float y, float w, float h) {
    b2BodyDef def = b2DefaultBodyDef();
    def.type = b2_staticBody;
    def.position = {x, y};

    auto body = std::make_unique<Box2dPhysicsBody>(world, def);
    b2Polygon shape = b2MakeBox(w / 2.0f, h / 2.0f);

    b2ShapeDef sdef = b2DefaultShapeDef();
    sdef.material.friction = 1.0f;
    b2CreatePolygonShape(body->getId(), &sdef, &shape);
    return body;
}

// --- Checkpoint  ---
std::unique_ptr<Box2dPhysicsBody> Box2DPhysicsFactory::createCheckpoint(b2WorldId world, float x, float y, float w, float h) {
    b2BodyDef def = b2DefaultBodyDef();
    def.type = b2_staticBody;
    def.position = {x, y};

    auto body = std::make_unique<Box2dPhysicsBody>(world, def);
    b2Polygon box =  b2MakeBox(w/2.0f, h/2.0f);
    b2ShapeDef sdef = b2DefaultShapeDef();

    sdef.isSensor = true; // no colisiona, solo detecta
    b2CreatePolygonShape(body->getId(), &sdef, &box);
    return body;
}

// --- Hint (flecha indicadora, solo visual o sensor TODO) ---
std::unique_ptr<Box2dPhysicsBody> Box2DPhysicsFactory::createHint(b2WorldId world, float x, float y, float w, float h) {
    b2BodyDef def = b2DefaultBodyDef();
    def.type = b2_staticBody;
    def.position = {x, y};

    auto body = std::make_unique<Box2dPhysicsBody>(world, def);
    b2Polygon shape = b2MakeBox(w / 2.0f, h / 2.0f);

    b2ShapeDef sdef = b2DefaultShapeDef();
    sdef.isSensor = true;
    b2CreatePolygonShape(body->getId(), &sdef, &shape);
    return body;
}

// --- Puente ---
std::unique_ptr<Box2dPhysicsBody> Box2DPhysicsFactory::createBridge(b2WorldId world, float x, float y, float w, float h, bool driveable) {
    b2BodyDef def = b2DefaultBodyDef();
    def.type = b2_staticBody;
    def.position = {x, y};

    auto body = std::make_unique<Box2dPhysicsBody>(world, def);
    b2Polygon shape = b2MakeBox(w / 2.0f, h / 2.0f);

    b2ShapeDef sdef = b2DefaultShapeDef();
    sdef.material.friction = driveable ? 0.7f : 1.0f;
    sdef.isSensor = !driveable; // los peatonales pueden ser sensores
    b2CreatePolygonShape(body->getId(), &sdef, &shape);
    return body;
}

