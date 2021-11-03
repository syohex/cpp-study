#include "json_object.h"

JsonObject::JsonObject(std::map<std::string, JsonValue *> value) : JsonValue(JsonType::kObject), value_(std::move(value)) {
}

JsonObject::~JsonObject() {
    for (const auto &it : value_) {
        delete it.second;
    }
}

bool JsonObject::IsObject() const noexcept {
    return true;
}

bool JsonObject::operator==(const JsonValue &value) const noexcept {
    if (type_ != value.Type()) {
        return false;
    }

    const auto &other = reinterpret_cast<const JsonObject &>(value);
    if (value_.size() != other.value_.size()) {
        return false;
    }

    for (const auto &it : value_) {
        const auto &key = it.first;
        if (other.value_.find(key) == other.value_.end()) {
            return false;
        }

        const auto *value = it.second;
        if (!(*value == *other.value_.at(key))) {
            return false;
        }
    }

    return true;
}
