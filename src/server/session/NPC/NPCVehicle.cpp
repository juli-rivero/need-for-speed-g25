#include "NPCVehicle.h"

#include <cmath>
#include <iostream>
#include <utility>

NPCVehicle::NPCVehicle(std::unique_ptr<Car>&& c) : car(std::move(c)) {}

void NPCVehicle::setTargetNode(const IntersectionNode* node) {
    previousNode = targetNode;
    targetNode = node;
}

void killLateralVelocity(Car* car) {
    auto body = car->getBody();
    Vec2 vel = body->getLinearVelocity();

    float angle = body->getAngle();
    b2Vec2 forward(std::cos(angle), std::sin(angle));

    // velocidades proyectadas
    float forwardVel = vel.x * forward.x + vel.y * forward.y;
    body->setLinearVelocity(forward.x * forwardVel, forward.y * forwardVel);
}

b2Vec2 NPCVehicle::steeringSeek(const b2Vec2& target) {
    auto pos = car->getBody()->getPosition();
    b2Vec2 desired = b2Vec2(target.x - pos.x, target.y - pos.y);

    float mag = std::sqrt(desired.x * desired.x + desired.y * desired.y);
    if (mag > 0.001f) {
        desired.x /= mag;
        desired.y /= mag;
    }

    desired.x *= 30.0f;
    desired.y *= 30.0f;

    auto vel = car->getBody()->getLinearVelocity();

    return b2Vec2(desired.x - vel.x, desired.y - vel.y);
}
NpcSnapshot NPCVehicle::buildSnapshot() const {
    const auto& body = car->getBody();
    const auto& [x, y] = body->getPosition();
    const auto& stats = car->getStaticStats();
    return {
        .type = car->getType(),
        .layer = car->getLayer(),
        .bound = {{x, y}, stats.width, stats.height},
        .angle = body->getAngle(),
    };
}

void NPCVehicle::update(float dt) {
    (void)dt;
    if (!targetNode) return;

    auto pos = car->getBody()->getPosition();
    killLateralVelocity(car.get());

    float dx = targetNode->pos.x - pos.x;
    float dy = targetNode->pos.y - pos.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    // Si ya llegó al nodo, no avanzar
    if (dist < 1.0f) {
        return;
    }

    b2Vec2 force = steeringSeek(targetNode->pos);
    car->getBody()->applyForce(force.x, force.y);

    //  Rota SOLO visualmente hacia el nodo (sin torque)
    float desiredAngle = std::atan2(dy, dx);
    float currentAngle = car->getBody()->getAngle();
    float delta = desiredAngle - currentAngle;

    // Normalizar delta a [-pi, pi]
    while (delta > M_PI) delta -= 2.0f * M_PI;
    while (delta < -M_PI) delta += 2.0f * M_PI;

    float rotateSpeed = 2.0f * dt;
    delta = std::clamp(delta, -rotateSpeed, rotateSpeed);

    float newAngle = currentAngle + delta;
    car->getBody()->setTransform(pos.x, pos.y, newAngle);
}
