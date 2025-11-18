#pragma once

#include <cxxopts.hpp>
#include <string>

class ArgsParser {
    cxxopts::Options options;
    cxxopts::ParseResult result;

   public:
    ArgsParser(int argc, const char* argv[]);

    std::string get_port() const;

    bool has_help() const;
    void print_help() const;

    void activate_logging() const;
};
