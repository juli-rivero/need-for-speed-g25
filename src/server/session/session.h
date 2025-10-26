#pragma once

#include <condition_variable>
#include <mutex>
#include <unordered_map>

#include "common/emitter.h"
#include "server/session/game.h"

class Session;

struct SessionListener : Listener<SessionListener> {
    Session& session;

    explicit SessionListener(Session& session);

    virtual void on_start_game(Game& game) = 0;
};

class Session final {
    static constexpr int MAX_USERS = 4;

    std::unordered_map<int, UserSetup> users_setup;
    std::mutex mtx;

    Game* game;

    friend struct SessionListener;
    Emitter<SessionListener> emitter;

   public:
    explicit Session(int creator);

    MAKE_FIXED(Session)

    uint16_t get_users_count();

    void add_client(int client_id);
    void remove_client(int client_id);

    bool in_game() const;

    void set_ready(int client_id, bool ready);

    ~Session();

   private:
    void start_game();

    bool all_ready() const;
};
