#include "client/connexion/mock_connexion_controller.h"

void MockConnexionController::send(const dto_session::SearchRequest&) {
    ResponseListener::on(dto_session::SearchResponse{
        .sessions = {{.session_id = "Session de Pedrito", .players_count = 3},
                     {.session_id = "Session de Carlos", .players_count = 5}}});
}

void MockConnexionController::send(const dto_session::JoinRequest&) {
    ResponseListener::on(dto_session::JoinResponse{});
}

void MockConnexionController::send(const dto_session::LeaveRequest&) {
    ResponseListener::on(dto_session::LeaveResponse{});
}

void MockConnexionController::send(const dto_lobby::StartRequest&) {
    ResponseListener::on(dto_lobby::StartResponse{});
}
