#include <exception>
#include <fstream>
#include <iostream>

#include "client/app.h"

#define HOST argv[1]
#define SERV argv[2]
#define FILE argv[3]

#define ERROR (-1)
#define SUCCESS 0

int main() {
    try {
        App app{};
        app.run();

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
