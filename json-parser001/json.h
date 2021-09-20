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

std::ostream& operator<<(std::ostream& os, const JsonToken& token);

std::tuple<std::vector<JsonToken>, std::string> lex(const std::string& input);
std::tuple<JsonValue, int, std::string> parse(const std::vector<JsonToken>& tokens, int index = 0);
std::tuple<JsonValue, std::string> parse(const std::string& input);
std::string deparse(const JsonValue& value, const std::string& whitespace = "");

} // namespace json