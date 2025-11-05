#include "server/clients_manager.h"

#include <utility>

#include "spdlog/spdlog.h"

void ClientsManager::manage_new_handler(Socket&& socket) {
    std::lock_guard lock(mutex);
    client_handlers.emplace_back(next_id++, std::move(socket),
                                 sessions_monitor);
}

void ClientsManager::reap_dead() {
    std::lock_guard lock(mutex);
    client_handlers.remove_if([](const ClientHandler& client_handler) {
        return not client_handler.is_alive();
    });
}

void ClientsManager::kill_all() {
    std::lock_guard lock(mutex);
    client_handlers.clear();
}

ClientsManager::~ClientsManager() { kill_all(); }
