#include "json_string.h"

JsonString::JsonString(std::string value) : JsonValue(JsonType::kString), value_(std::move(value)) {
}

bool JsonString::IsString() const noexcept {
    return true;
}

bool JsonString::operator==(const JsonValue &value) const noexcept {
    if (type_ != value.Type()) {
        return false;
    }

    const auto &other = reinterpret_cast<const JsonString &>(value);
    return value_ == other.value_;
}

std::string JsonString::Value() const noexcept {
    return value_;
}
