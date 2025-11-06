#pragma once

#include <common/structs.h>

#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "common/emitter.h"
#include "server/session/game.h"
#include "spdlog/spdlog.h"

class Session;

class Session final {
    const SessionConfig config;

    std::shared_ptr<spdlog::logger> log;

    std::unordered_map<int, UserSetup> users_setup;
    std::mutex mtx;

    Game* game;

   public:
    struct Listener : common::Listener<Session::Listener> {
        Session& session;

        explicit Listener(Session& session);

        virtual void on_start_game(Game& game) = 0;
    };

    explicit Session(const SessionConfig&, int creator);

    MAKE_FIXED(Session)

    SessionInfo get_info();

    void add_client(int client_id);
    void remove_client(int client_id);

    bool in_game() const;
    bool full() const;

    void set_ready(int client_id, bool ready);

    ~Session();

   private:
    common::Emitter<Session::Listener> emitter;

    void start_game();

    bool all_ready() const;
};
