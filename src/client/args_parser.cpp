#include "client/args_parser.h"

#include <iostream>
#include <ostream>

ArgsParser::ArgsParser(const int argc, const char* argv[])
    : options("client",
              "A client application that connects to the game server to play "
              "'Need for Speed' with other users."),
      result([this, argc, argv]() {
          options.add_options()(
              "h,host", "Host where the server is running",
              cxxopts::value<std::string>()->default_value("localhost"))(
              "p,port", "Port where the server is running",
              cxxopts::value<std::string>()->default_value("4657"))(
              "help", "Show help");
          return options.parse(argc, argv);
      }()) {
    // ✅ Verificar opciones no reconocidas manualmente
    if (!result.unmatched().empty()) {
        throw cxxopts::exceptions::parsing("Unrecognized: " +
                                           result.unmatched()[0]);
    }
}

std::string ArgsParser::get_host() const {
    return result["host"].as<std::string>();
}
std::string ArgsParser::get_port() const {
    return result["port"].as<std::string>();
}
bool ArgsParser::has_help() const { return result.count("help"); }
void ArgsParser::print_help() const {
    std::cout << options.help() << std::endl;
}
