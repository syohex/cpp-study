#include "json_number.h"

JsonNumber::JsonNumber(double value) : JsonValue(JsonType::kNumber), value_(value) {
}

bool JsonNumber::IsNumber() const noexcept {
    return true;
}

bool JsonNumber::operator==(const JsonValue &value) const noexcept {
    if (type_ != value.Type()) {
        return false;
    }

    const auto &other = reinterpret_cast<const JsonNumber &>(value);
    return value_ == other.value_;
}

double JsonNumber::Value() const noexcept {
    return value_;
}
