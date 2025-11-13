#pragma once

#include <common/structs.h>

#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/emitter.h"
#include "common/macros.h"
#include "logic/GameSessionFacade.h"
#include "server/config/YamlGameConfig.h"
#include "spdlog/spdlog.h"

class Session;

struct UserSetup {
    PlayerConfig game_config;
    bool ready{false};

    UserSetup(const std::string& name, PlayerId id);
};

class Session final {
    const SessionConfig config;

    std::shared_ptr<spdlog::logger> log;

    std::unordered_map<PlayerId, UserSetup> users_setup;
    std::mutex mtx;

    GameSessionFacade game;

    YamlGameConfig& yaml_config;

    const PlayerId creator;

   public:
    struct Listener : common::Listener<Session::Listener> {
        Session& session;

        explicit Listener(Session& session);

        virtual void on_start_game(GameSessionFacade& game) = 0;
        virtual void on_session_updated(
            const SessionConfig&, const std::vector<PlayerInfo>& players) = 0;
    };

    Session(const SessionConfig&, PlayerId creator, YamlGameConfig&);

    MAKE_FIXED(Session)

    SessionInfo get_info();

    void add_client(PlayerId client_id);
    void remove_client(PlayerId client_id);

    bool in_game() const;
    bool full() const;
    bool empty() const;

    std::vector<CarStaticInfo> get_types_of_static_cars() const;

    void set_car(PlayerId client_id, const std::string& car_name);

    void set_ready(PlayerId client_id, bool ready);

    ~Session();

   private:
    common::Emitter<Session::Listener> emitter;

    void start_game();

    bool all_ready() const;

    void notify_change();
};
