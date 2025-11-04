#include "client/connexion/request_controller.h"

RequestController::RequestController(Sender& sender) : sender(sender) {}
void RequestController::request_all_sessions() {
    sender.send(dto_session::SearchRequest{});
}
void RequestController::request_join_session(const std::string& session_id) {
    sender.send(dto_session::JoinRequest{session_id});
}
void RequestController::request_leave_current_session() {
    sender.send(dto_session::LeaveRequest{});
}
void RequestController::request_start_game() {
    sender.send(dto_lobby::StartRequest{});
}
