#include "client/connexion/mock_connexion_controller.h"

#include "common/dto/dto_session.h"

void MockConnexionController::request_all_sessions() {
    recv(dto_session::SearchResponse{{
        SessionInfo{
            .name = "Los menos capos",
            .maxPlayers = 2,
            .raceCount = 3,
            .city = "New York",
            .currentPlayers = 1,
            .status = SessionStatus::Waiting,
        },
        SessionInfo{
            .name = "Los mas capos",
            .maxPlayers = 3,
            .raceCount = 3,
            .city = "Buenos Aires",
            .currentPlayers = 3,
            .status = SessionStatus::Full,
        },
        SessionInfo{
            .name = "Partida de Pablo",
            .maxPlayers = 5,
            .raceCount = 2,
            .city = "Rosario",
            .currentPlayers = 5,
            .status = SessionStatus::Playing,
        },
    }});
}
void MockConnexionController::request_join_session(const std::string&) {
    recv(dto_session::JoinResponse{});
}
void MockConnexionController::request_leave_current_session() {
    recv(dto_session::LeaveResponse{});
}
void MockConnexionController::request_start_game() {
    recv(dto_lobby::StartResponse{});
}
