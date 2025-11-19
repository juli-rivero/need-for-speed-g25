#include "server/args_parser.h"

#include <spdlog/spdlog.h>

#include <iostream>
#include <ostream>
#include <utility>
#include <vector>

ArgsParser::ArgsParser(const int argc, const char* argv[])
    : options("client",
              "A client application that connects to the game server to play "
              "'Need for Speed' with other users."),
      result([this, argc, argv]() {
          options.add_options("Network")(
              "p,port", "Port where the server is running",
              cxxopts::value<std::string>()->default_value("4657"));
          auto add_log_flag =
              options.add_options("Logging: Each level includes below levels");
          add_log_flag("trace", "Enable verbose debugging");
          add_log_flag("debug", "Enable normal debugging");
          add_log_flag("info", "Enable normal info (default)");
          add_log_flag("warn", "Enable warnings");
          add_log_flag("error", "Enable recoverable errors");
          add_log_flag("critical", "Enable serious errors");
          options.add_options("General")("help", "Show help");
          return options.parse(argc, argv);
      }()) {
    // ✅ Verificar opciones no reconocidas manualmente
    if (!result.unmatched().empty()) {
        throw cxxopts::exceptions::parsing("Unrecognized: " +
                                           result.unmatched()[0]);
    }
}

std::string ArgsParser::get_port() const {
    return result["port"].as<std::string>();
}
bool ArgsParser::has_help() const { return result.count("help"); }
void ArgsParser::print_help() const {
    std::cout << options.help() << std::endl;
}

void ArgsParser::activate_logging() const {
    const std::vector<std::pair<std::string, spdlog::level::level_enum>>
        log_levels = {
            {"trace", spdlog::level::trace},
            {"debug", spdlog::level::debug},
            {"info", spdlog::level::info},
            {"warn", spdlog::level::warn},
            {"error", spdlog::level::err},
            {"critical", spdlog::level::critical},
        };

    bool active_flag = false;
    spdlog::level::level_enum level = spdlog::level::info;
    for (auto& [name, lvl] : log_levels) {
        if (result.count(name)) {
            if (active_flag) {
                throw cxxopts::exceptions::parsing(
                    "multiple log levels specified (choose only one)");
            }
            active_flag = true;
            level = lvl;
        }
    }

    spdlog::set_level(level);
    spdlog::set_pattern("[%T] [%n] [%^%l%$] %v");
}
