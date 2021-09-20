#include "json.h"

#include <sstream>
#include <cctype>

namespace json {

namespace {

bool is_syntax_character(char c) {
    return c == '[' || c == ']' || c == '{' || c == '}' || c == ':' || c == ',';
}

std::tuple<JsonToken, size_t, std::string> lex_string(const std::string &raw_json, size_t index) {
    size_t pos = index;
    JsonToken token{"", JsonTokenType::kString, index};

    ++pos; // start quote
    char c;
    while ((c = raw_json[pos]) != '"') {
        if (pos == raw_json.size()) {
            return {token, pos, "missing string end quote"};
        }

        token.value += c;
        ++pos;
    }

    ++pos; // end quote
    return {token, pos, ""};
}

std::tuple<JsonToken, size_t, std::string> lex_number(const std::string &raw_json, size_t index) {
    size_t pos = index;
    JsonToken token{"", JsonTokenType::kNumber, pos};

    size_t limit = raw_json.size();
    while (pos < limit) {
        char c = raw_json[pos];
        if (!(c >= '0' && c <= '9')) {
            break;
        }

        token.value += c;
        ++pos;
    }

    return {token, pos, ""};
}

bool match_keyword(const std::string &raw_json, size_t index, const std::string &keyword) {
    if (raw_json.size() - index < keyword.size()) {
        return false;
    }

    return raw_json.substr(index, keyword.size()) == keyword;
}

std::string format_error(const std::string &message, size_t pos) {
    return message + "at " + std::to_string(pos);
}

std::string token_type_to_string(JsonTokenType type) {
    switch (type) {
    case JsonTokenType::kBoolean:
        return "boolean";
    case JsonTokenType::kNull:
        return "null";
    case JsonTokenType::kNumber:
        return "number";
    case JsonTokenType::kString:
        return "string";
    case JsonTokenType::kSyntax:
        return "syntax";
    }
}

} // namespace

std::ostream &operator<<(std::ostream &os, const JsonToken &token) {
    os << "JsonToken{ value=" << token.value << ", type=" << token_type_to_string(token.type) << ", at=" << token.location;
    return os;
}

std::tuple<std::vector<JsonToken>, std::string> lex(const std::string &raw_json) {
    std::vector<JsonToken> tokens;
    size_t limit = raw_json.size();

    size_t i = 0;
    while (i < limit) {
        // skip spaces
        while (i < limit && std::isspace(raw_json[i])) {
            ++i;
        }
        if (i == limit) {
            break;
        }

        char c = raw_json[i];
        if (is_syntax_character(c)) {
            tokens.push_back({std::string(1, c), JsonTokenType::kSyntax, i});
            ++i;
            continue;
        } else if (c == '"') {
            auto [token, new_index, error] = lex_string(raw_json, i);
            if (!error.empty()) {
                return {{}, format_error("failed to lex string: " + error, i)};
            }

            tokens.push_back(token);
            i = new_index;
            continue;
        } else if (c >= '0' && c <= '9') {
            auto [token, new_index, error] = lex_number(raw_json, i);
            if (!error.empty()) {
                return {{}, format_error("failed to lex number: " + error, i)};
            }

            tokens.push_back(token);
            i = new_index;
            continue;
        } else if (c == 'n') {
            if (!match_keyword(raw_json, i, "null")) {
                return {{}, format_error("found unknown literal", i)};
            }

            tokens.push_back({"null", JsonTokenType::kNull, i});
            i += 4;
            continue;
        } else if (c == 't') {
            if (!match_keyword(raw_json, i, "true")) {
                return {{}, format_error("found unknown literal", i)};
            }

            tokens.push_back({"true", JsonTokenType::kBoolean, i});
            i += 4;
            continue;
        } else if (c == 'f') {
            if (!match_keyword(raw_json, i, "false")) {
                return {{}, format_error("found unknown literal", i)};
            }

            tokens.push_back({"false", JsonTokenType::kBoolean, i});
            i += 5;
            continue;
        }

        return {{}, format_error("found unknown error", i)};
    }

    return {tokens, ""};
}

std::string deparse(const JsonValue &value, const std::string &whitespace) {
    switch (value.type) {
    case JsonValueType::kString:
        return "\"" + value.string.value() + "\"";
    case JsonValueType::kBoolean:
        return value.boolean.value() ? "true" : "false";
    case JsonValueType::kNumber:
        return std::to_string(value.number.value());
    case JsonValueType::kNull:
        return "null";
    case JsonValueType::kArray: {
        std::stringstream ss;
        ss << "[\n";

        const auto &array = value.array.value();
        for (size_t i = 0; i < array.size(); ++i) {
            ss << whitespace << " " << deparse(array[i], whitespace + " ");
            if (i < array.size() - 1) {
                ss << ",";
            }
            ss << "\n";
        }

        ss << whitespace << "\n";
        return ss.str();
    }
    case JsonValueType::kObject: {
        std::stringstream ss;
        ss << "{"
           << "\n";

        int i = 0;
        const auto &object = value.object.value();
        for (const auto &[key, value] : object) {
            ss << whitespace << " "
               << "\"" << key << "\": " << deparse(value, whitespace + " ");

            if (i < object.size() - 1) {
                ss << ",";
            }

            ss << "\n";
            ++i;
        }

        ss << whitespace << "}";
        return ss.str();
    }
    }
}

} // namespace json