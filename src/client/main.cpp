#include <spdlog/spdlog.h>

#include <cxxopts.hpp>
#include <exception>

#include "client/app.h"
#include "client/args_parser.h"

#define HOST argv[1]
#define SERV argv[2]
#define FILE argv[3]

#define ERROR (-1)
#define SUCCESS 0

int main(const int argc, const char* argv[]) {
    try {
        const ArgsParser args_parser(argc, argv);

        args_parser.activate_logging();

        if (args_parser.has_help()) {
            args_parser.print_help();
            return SUCCESS;
        }

        spdlog::info("Connecting to " + args_parser.get_host() + ":" +
                     args_parser.get_port());

        App app(args_parser.get_host(), args_parser.get_port());
        app.run();

        return SUCCESS;
    } catch (const cxxopts::exceptions::parsing& e) {
        spdlog::critical(e.what());
        spdlog::info("Use flag --help for more info");

        return ERROR;
    } catch (const std::exception& err) {
        spdlog::critical("Something went wrong and an exception was caught: {}",
                         err.what());
        return ERROR;
    } catch (...) {
        spdlog::critical(
            "Something went wrong and an unknown exception was caught.");
        return ERROR;
    }
}
