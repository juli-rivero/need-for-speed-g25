#include "common/dto/dto_game.h"

#include "common/dto/structs_serializable.h"

using namespace dto_game;

// GameStaticSnapshot SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, GameStaticSnapshot& r) {
    return p >> r.new_race_info;
}
ProtocolSender& operator<<(ProtocolSender& p, const GameStaticSnapshot& r) {
    return p << r.new_race_info;
}

// TurnRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, TurnRequest& e) {
    return p >> e.direction >> e.turn;
}
ProtocolSender& operator<<(ProtocolSender& p, const TurnRequest& e) {
    return p << e.direction << e.turn;
}

// AccelerateRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, AccelerateRequest& e) {
    return p >> e.accelerate;
}
ProtocolSender& operator<<(ProtocolSender& p, const AccelerateRequest& e) {
    return p << e.accelerate;
}

// UseBoostRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, UseBoostRequest& e) {
    return p >> e.useBoost;
}
ProtocolSender& operator<<(ProtocolSender& p, const UseBoostRequest& e) {
    return p << e.useBoost;
}

// ReverseRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, ReverseRequest& e) {
    return p >> e.reverse;
}
ProtocolSender& operator<<(ProtocolSender& p, const ReverseRequest& e) {
    return p << e.reverse;
}

// GameSnapshot SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, GameSnapshotPacket& packet) {
    return p >> packet.snapshot;
}
ProtocolSender& operator<<(ProtocolSender& p,
                           const GameSnapshotPacket& packet) {
    return p << packet.snapshot;
}

ProtocolSender& operator<<(ProtocolSender& p, const EventPacket& r) {
    return p << r.collision;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, EventPacket& r) {
    return p >> r.collision;
}
ProtocolSender& operator<<(ProtocolSender& p, const dto_game::CheatMessage& c) {
    return p << c.type;
}
ProtocolReceiver& operator>>(ProtocolReceiver& p, dto_game::CheatMessage& c) {
    return p >> c.type;
}
