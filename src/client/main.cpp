#include <cxxopts.hpp>
#include <exception>
#include <fstream>
#include <iostream>

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

        if (args_parser.has_help()) {
            args_parser.print_help();
            return SUCCESS;
        }

        std::cout << "Connecting to " << args_parser.get_host() << ":"
                  << args_parser.get_port() << std::endl;

        App app{};
        app.run();

        return SUCCESS;
    } catch (const cxxopts::exceptions::parsing& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Use option --help for more info" << std::endl;

        return ERROR;
    } catch (const std::exception& err) {
        std::cerr << "Something went wrong and an exception was caught: "
                  << err.what() << "\n";
        return ERROR;
    } catch (...) {
        std::cerr
            << "Something went wrong and an unknown exception was caught.\n";
        return ERROR;
    }
}
