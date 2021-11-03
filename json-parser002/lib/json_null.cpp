#include "json_null.h"

JsonNull::JsonNull() : JsonValue(JsonType::kNull) {
}

bool JsonNull::IsNull() const noexcept {
    return true;
}
bool JsonNull::operator==(const JsonValue &value) const noexcept {
    return type_ == value.Type();
}

std::nullptr_t JsonNull::Value() const noexcept {
    return nullptr;
}
