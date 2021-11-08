#include "command_line_parser.h"

CommandLineError::CommandLineError(std::string message) : message_(std::move(message)) {
}

const char *CommandLineError::what() const noexcept {
    return message_.c_str();
}

CommandLineParser::~CommandLineParser() {
    for (auto &it : options_) {
        delete it.second;
    }
}

void CommandLineParser::Add(const std::string &long_name, char short_name, const std::string &description) {
    if (options_.find(long_name) != options_.end()) {
        throw CommandLineError("Option: " + long_name + "is already defined");
    }

    options_[long_name] = nullptr;
}