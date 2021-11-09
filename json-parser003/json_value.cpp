#include "json_value.h"

JsonValue::JsonValue() : JsonValue(JsonType::kNull) {
}

JsonValue::JsonValue(std::nullptr_t value) : JsonValue(JsonType::kNull) {
}

JsonValue::JsonValue(JsonType type) : type_(type), u_({}) {
    switch (type_) {
    case JsonType::kBoolean:
        u_.boolean_ = false;
        break;
    case JsonType::kNumber:
        u_.number_ = 0.0;
        break;
    case JsonType::kInteger:
        u_.int64_ = 0;
        break;
    case JsonType::kString:
        u_.string_ = new std::string();
        break;
    case JsonType::kArray:
        u_.array_ = new JsonArray();
        break;
    case JsonType::kObject:
        u_.object_ = new JsonObject();
        break;
    default:
        break;
    }
}

JsonValue::JsonValue(bool value) : type_(JsonType::kBoolean), u_({}) {
    u_.boolean_ = value;
}

JsonValue::JsonValue(double value) : type_(JsonType::kNumber), u_({}) {
    u_.number_ = value;
}

JsonValue::JsonValue(std::int64_t value) : type_(JsonType::kInteger), u_({}) {
    u_.int64_ = value;
}

JsonValue::JsonValue(const std::string &value) : type_(JsonType::kString), u_({}) {
    u_.string_ = new std::string(value);
}

JsonValue::JsonValue(std::string &&value) : type_(JsonType::kString), u_({}) {
    u_.string_ = new std::string(std::move(value));
}

JsonValue::JsonValue(const char *value) : type_(JsonType::kString), u_({}) {
    u_.string_ = new std::string(value);
}

JsonValue::JsonValue(const JsonArray &value) : type_(JsonType::kArray), u_({}) {
    u_.array_ = new JsonArray(value);
}

JsonValue::JsonValue(JsonArray &&value) : type_(JsonType::kArray), u_({}) {
    u_.array_ = new JsonArray(std::move(value));
}

JsonValue::JsonValue(const JsonObject &value) : type_(JsonType::kObject), u_({}) {
    u_.object_ = new JsonObject(value);
}

JsonValue::JsonValue(JsonObject &&value) : type_(JsonType::kObject), u_({}) {
    u_.object_ = new JsonObject(std::move(value));
}

JsonValue::JsonValue(const JsonValue &other) : type_(other.type_), u_({}) {
    switch (type_) {
    case JsonType::kString:
        u_.string_ = new std::string(*other.u_.string_);
        break;
    case JsonType::kArray:
        u_.array_ = new JsonArray(*other.u_.array_);
        break;
    case JsonType::kObject:
        u_.object_ = new JsonObject(*other.u_.object_);
        break;
    default:
        u_ = other.u_;
        break;
    }
}

JsonValue &JsonValue::operator=(const JsonValue &other) {
    if (this != &other) {
        JsonValue tmp(other);
        std::swap(type_, tmp.type_);
        std::swap(u_, tmp.u_);
    }

    return *this;
}

JsonValue::JsonValue(JsonValue &&other) noexcept {
    std::swap(type_, other.type_);
    std::swap(u_, other.u_);
}

JsonValue &JsonValue::operator=(JsonValue &&other) noexcept {
    std::swap(type_, other.type_);
    std::swap(u_, other.u_);
    return *this;
}

JsonValue::~JsonValue() {
    Clear();
}

void JsonValue::Clear() {
    switch (type_) {
    case JsonType::kString:
        delete u_.string_;
        break;
    case JsonType::kArray:
        delete u_.array_;
        break;
    case JsonType::kObject:
        delete u_.object_;
        break;
    default:
        break;
    }
}

bool JsonValue::IsNull() const noexcept {
    return type_ == JsonType::kNull;
}

bool JsonValue::IsBoolean() const noexcept {
    return type_ == JsonType::kBoolean;
}

bool JsonValue::IsNumber() const noexcept {
    return type_ == JsonType::kNumber || type_ == JsonType::kInteger;
}

bool JsonValue::IsInteger() const noexcept {
    return type_ == JsonType::kInteger;
}

bool JsonValue::IsString() const noexcept {
    return type_ == JsonType::kString;
}

bool JsonValue::IsArray() const noexcept {
    return type_ == JsonType::kArray;
}

bool JsonValue::IsObject() const noexcept {
    return type_ == JsonType::kObject;
}

JsonType JsonValue::Type() const noexcept {
    return type_;
}