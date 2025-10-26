#include <exception>
#include <fstream>
#include <iostream>

#include "server/clients_manager.h"
#include "server/server.h"

#define PORT argv[1]

#define ERROR (-1)
#define SUCCESS 0

int main(const int argc, const char* argv[]) {
    try {
        if (argc != 2) {
            throw std::runtime_error(
                "Bad program call. Expected \n"
                "\t./server <puerto>\n");
        }

        const Server server(PORT);

        while (std::cin.get() != 'q') {
        }

        return SUCCESS;
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
