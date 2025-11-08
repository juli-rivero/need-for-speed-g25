#include "server/acceptor.h"

#include <sys/socket.h>

#include <iostream>
#include <string>
#include <utility>

#include "../common/liberror.h"
#include "spdlog/sinks/stdout_color_sinks-inl.h"
#include "spdlog/spdlog.h"

using spdlog::stdout_color_mt;

Acceptor::Acceptor(const std::string& port, ClientsManager& clients_manager)
    : acceptor(Socket(port.c_str())), clients_manager(clients_manager) {
    const auto log = stdout_color_mt("Acceptor");
    log->info("Acceptor started on port {}", port);
}

void Acceptor::run() {
    while (should_keep_running()) {
        try {
            Socket socket = acceptor.accept();
            clients_manager.reap_dead();
            clients_manager.manage_new_handler(std::move(socket));
        } catch (LibError& e) {
            if (not should_keep_running()) break;  // no error happened
            std::cerr << e.what() << std::endl;
        }
    }
}
void Acceptor::stop() {
    Thread::stop();
    acceptor.shutdown(SHUT_RDWR);
}
