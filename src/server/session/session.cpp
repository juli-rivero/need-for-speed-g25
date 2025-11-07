#include "../../server/session/session.h"

#include <ranges>

#include "spdlog/sinks/stdout_color_sinks-inl.h"
#include "spdlog/spdlog.h"

using spdlog::stdout_color_mt;

SessionListener::SessionListener(Session& session)
    : Listener(session.emitter), session(session) {}

Session::Session(const std::string& session_id, const int creator)
    : log(stdout_color_mt("Session " + session_id)),
      users_setup(1),
      game(nullptr) {
    log->debug("Created");
    add_client(creator);
}

uint16_t Session::get_users_count() {
    std::lock_guard lock(mtx);
    log->trace("get_users_count: {}", users_setup.size());
    return users_setup.size();
}

void Session::add_client(const int client_id) {
    std::lock_guard lock(mtx);
    if (in_game()) throw std::runtime_error("Game already started");
    if (users_setup.size() >= MAX_USERS) {
        throw std::runtime_error("Session is full");
    }
    users_setup[client_id] = {};
    log->debug("client {} joined", client_id);
}

void Session::remove_client(const int client_id) {
    std::lock_guard lock(mtx);
    if (not in_game()) users_setup.erase(client_id);
    log->debug("client {} left", client_id);
}

bool Session::in_game() const { return game != nullptr; }

void Session::set_ready(const int client_id, const bool ready) {
    log->debug("client {} set ready to {}", client_id, ready);
    std::lock_guard lock(mtx);
    if (in_game()) throw std::runtime_error("Game already started");
    users_setup.at(client_id).ready = ready;
    if (all_ready()) start_game();
}

Session::~Session() {
    std::unique_lock lock(mtx);
    delete game;
    log->debug("Destroyed");
}

void Session::start_game() {
    std::lock_guard lock(mtx);
    game = new Game(users_setup, log.get());
    emitter.emit(&SessionListener::on_start_game, *game);
}

bool Session::all_ready() const {
    for (const auto& user : users_setup | std::views::values) {
        if (not user.ready) return false;
    }
    return true;
}
