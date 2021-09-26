#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace json {

enum class JsonValueType {
    kString,
    kNumber,
    kObject,
    kArray,
    kBoolean,
    kNull,
};

struct JsonValue {
    std::optional<std::string> string;
    std::optional<double> number;
    std::optional<bool> boolean;
    std::optional<std::vector<JsonValue>> array;
    std::optional<std::map<std::string, JsonValue>> object;
    JsonValueType type;

    JsonValue() : type(JsonValueType::kNull) {
    }

    explicit JsonValue(std::string str) : string(std::move(str)), type(JsonValueType::kString) {
    }
    explicit JsonValue(const char *str) : string(str), type(JsonValueType::kString) {
    }
    explicit JsonValue(double num) : number(num), type(JsonValueType::kNumber) {
    }
    explicit JsonValue(bool b) : boolean(b), type(JsonValueType::kBoolean) {
    }
    explicit JsonValue(std::vector<JsonValue> arr) : array(std::move(arr)), type(JsonValueType::kArray) {
    }
    explicit JsonValue(std::map<std::string, JsonValue> m) : object(std::move(m)), type(JsonValueType::kObject) {
    }

    bool operator==(const JsonValue &other) const {
        if (type != other.type) {
            return false;
        }

        switch (type) {
        case JsonValueType::kString:
            return string == other.string;
        case JsonValueType::kNumber:
            return number == other.number;
        case JsonValueType::kBoolean:
            return boolean == other.boolean;
        case JsonValueType::kArray:
            return array == other.array;
        case JsonValueType::kObject:
            return object == other.object;
        default:
            return true;
        }
    }

    bool operator!=(const JsonValue &other) const {
        return !(*this == other);
    }
};

enum class JsonTokenType {
    kString,
    kNumber,
    kSyntax,
    kBoolean,
    kNull,
};

struct JsonToken {
    std::string value;
    JsonTokenType type;
    size_t location;
};

std::ostream &operator<<(std::ostream &os, const JsonToken &token);

std::tuple<std::vector<JsonToken>, std::string> lex(const std::string &input);
std::tuple<JsonValue, size_t, std::string> parse(std::vector<JsonToken> &tokens, size_t index = 0);
std::tuple<JsonValue, std::string> parse(const std::string &input);
std::string deparse(const JsonValue &value, const std::string &whitespace = "");

} // namespace json