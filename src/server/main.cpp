#include <exception>
#include <fstream>
#include <iostream>

#include "client/args_parser.h"
#include "server/server.h"
#include "spdlog/spdlog.h"
//#include "server/test_simulador.cpp"

#define PORT argv[1]

#define ERROR (-1)
#define SUCCESS 0

int main(const int argc, const char* argv[]) {
    //test();
    try {
        const ArgsParser args_parser(argc, argv);

        args_parser.activate_logging();

        if (args_parser.has_help()) {
            args_parser.print_help();
            return SUCCESS;
        }

        const Server server(args_parser.get_port());

        while (std::cin.get() != 'q') {
        }

        return SUCCESS;
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