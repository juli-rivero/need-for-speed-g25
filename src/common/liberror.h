#pragma once

#include <cstring>
#include <exception>
#include <format>
#include <string>
#include <string_view>
#include <system_error>

class LibError final : public std::exception {
    std::string msg_error;

   public:
    template <typename... Args>
    LibError(const int error_code, const std::string_view fmt,
             Args&&... args) noexcept {
        try {
            msg_error = std::vformat(fmt, std::make_format_args(args...));
        } catch (...) {
            msg_error = "???";
        }

        char err_buf[128];
        if (not strerror_r(error_code, err_buf, sizeof(err_buf))) {
            msg_error += " ";
            msg_error += err_buf;
        } else {
            msg_error += " <unknown error>";
        }
    }

    const char* what() const noexcept override { return msg_error.c_str(); }

    ~LibError() noexcept override = default;
};
