#include "json_boolean.h"

JsonBoolean::JsonBoolean(bool value) : JsonValue(JsonType::kBoolean), value_(value) {
}

bool JsonBoolean::IsBoolean() const noexcept {
    return true;
}

bool JsonBoolean::operator==(const JsonValue &value) const noexcept {
    if (type_ != value.Type()) {
        return false;
    }

    const auto &other = reinterpret_cast<const JsonBoolean &>(value);
    return value_ == other.value_;
}
bool JsonBoolean::Value() const noexcept {
    return value_;
}
