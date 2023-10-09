#pragma once

#include <string>
#include <stdexcept>

namespace sender {

struct SenderError : public std::runtime_error {
    SenderError(std::string const &msg) : std::runtime_error(msg) {}
};

std::string add_file(const std::string &filename, const std::string &how_to_run);
void open_file(const std::string &filename);

} // namespace sender
