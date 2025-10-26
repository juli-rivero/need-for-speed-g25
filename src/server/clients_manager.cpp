#include "server/clients_manager.h"

#include <utility>

void ClientsManager::manage_new_handler(Socket&& socket) {
    std::lock_guard lock(mutex);
    client_handlers.emplace_back(next_id++, std::move(socket),
                                 sessions_monitor);
}

void ClientsManager::reap_dead() {
    std::lock_guard lock(mutex);
    client_handlers.remove_if([](ClientHandler& client_handler) {
        if (not client_handler.is_alive()) {
            client_handler.join();
            return true;
        }
        return false;
    });
}
void ClientsManager::kill_all() {
    std::lock_guard lock(mutex);
    for (auto& client_handler : client_handlers) {
        if (client_handler.is_alive()) {
            client_handler.kill();
        }
        client_handler.join();
    }
    client_handlers.clear();
}

ClientsManager::~ClientsManager() { kill_all(); }
