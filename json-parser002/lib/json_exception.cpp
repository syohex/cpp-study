#include "json_exception.h"

#include <sstream>

JsonException::JsonException(const std::string &message, int line, int column) {
    std::stringstream ss;
    ss << message << " at " << line << ':' << column;
}
const char *JsonException::what() const noexcept {
    return message_.c_str();
}
