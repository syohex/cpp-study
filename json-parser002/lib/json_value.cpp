#include "json_value.h"

#include "json_array.h"
#include "json_boolean.h"
#include "json_number.h"
#include "json_object.h"
#include "json_string.h"

JsonValue::JsonValue(JsonType type) : type_(type) {
}

bool JsonValue::IsObject() const noexcept {
    return false;
}

bool JsonValue::IsArray() const noexcept {
    return false;
}

bool JsonValue::IsNumber() const noexcept {
    return false;
}

bool JsonValue::IsString() const noexcept {
    return false;
}

bool JsonValue::IsBoolean() const noexcept {
    return false;
}

bool JsonValue::IsNull() const noexcept {
    return false;
}

JsonType JsonValue::Type() const noexcept {
    return type_;
}

bool JsonValue::operator==(const JsonValue &other) const {
    if (type_ != other.type_) {
        return false;
    }

    switch (type_) {
    case JsonType::kObject: {
        const auto &self = reinterpret_cast<const JsonObject &>(*this);
        return self == other;
    }
    case JsonType::kArray: {
        const auto &self = reinterpret_cast<const JsonArray &>(*this);
        return self == other;
    }
    case JsonType::kNumber: {
        const auto &self = reinterpret_cast<const JsonNumber &>(*this);
        return self == other;
    }
    case JsonType::kString: {
        const auto &self = reinterpret_cast<const JsonString &>(*this);
        return self == other;
    }
    case JsonType::kBoolean: {
        const auto &self = reinterpret_cast<const JsonBoolean &>(*this);
        return self == other;
    }
    case JsonType::kNull:
        return true;
    }
}
