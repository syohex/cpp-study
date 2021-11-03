#include "json_array.h"

JsonArray::JsonArray(std::vector<JsonValue *> value) : JsonValue(JsonType::kArray), value_(std::move(value)) {
}

JsonArray::~JsonArray() {
    for (auto *v : value_) {
        delete v;
    }
}

bool JsonArray::IsArray() const noexcept {
    return true;
}
bool JsonArray::operator==(const JsonValue &value) const noexcept {
    if (type_ != value.Type()) {
        return false;
    }

    const auto &other = reinterpret_cast<const JsonArray &>(value);
    if (value_.size() != other.value_.size()) {
        return false;
    }

    for (size_t i = 0; i < value_.size(); ++i) {
        if (!(*value_[i] == *other.value_[i])) {
            return false;
        }
    }

    return true;
}
