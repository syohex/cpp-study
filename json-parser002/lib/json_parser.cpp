#include "json_parser.h"

#include <cstdlib>
#include <sstream>

#include "json_array.h"
#include "json_boolean.h"
#include "json_exception.h"
#include "json_null.h"
#include "json_number.h"
#include "json_object.h"
#include "json_string.h"

namespace {

bool IsHexDigit(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

} // namespace

JsonParser::JsonParser() : current_(0), index_(0), limit_(0), line_(1), column_(1) {
}

JsonValue *JsonParser::Parse(const std::string &input) {
    buffer_ = input;
    limit_ = input.size();
    index_ = 0;
    line_ = 1;
    column_ = 1;

    Read();
    SkipWhiteSpace();
    JsonValue *value = ReadValue();
    SkipWhiteSpace();

    return value;
}

void JsonParser::Read() {
    if (index_ == limit_) {
        return;
    }

    if (current_ == '\n') {
        ++line_;
        column_ = 0;
    }

    ++column_;

    current_ = buffer_[index_];
    ++index_;
}

bool JsonParser::ReadChar(char ch) {
    if (current_ != ch) {
        return false;
    }

    Read();
    return true;
}

void JsonParser::ReadRequiredChar(char ch) {
    if (!ReadChar(ch)) {
        std::stringstream ss;
        ss << "Unexpected character: " << current_ << " Expected: " << ch;
        throw JsonException(ss.str(), line_, column_);
    }
}

JsonValue *JsonParser::ReadValue() {
    switch (current_) {
    case 'n':
        return ReadNull();
    case 't':
        return ReadTrue();
    case 'f':
        return ReadFalse();
    case '"':
        return ReadString();
    case '[':
        return ReadArray();
    case '{':
        return ReadObject();
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return ReadNumber();
    default: {
        throw JsonException("Unexpected value", line_, column_);
    }
    }
}

JsonValue *JsonParser::ReadNull() {
    Read();
    ReadRequiredChar('u');
    ReadRequiredChar('l');
    ReadRequiredChar('l');

    return new JsonNull();
}

JsonValue *JsonParser::ReadTrue() {
    Read();
    ReadRequiredChar('r');
    ReadRequiredChar('u');
    ReadRequiredChar('e');

    return new JsonBoolean(true);
}

JsonValue *JsonParser::ReadFalse() {
    Read();
    ReadRequiredChar('a');
    ReadRequiredChar('l');
    ReadRequiredChar('s');
    ReadRequiredChar('e');

    return new JsonBoolean(false);
}

JsonValue *JsonParser::ReadNumber() {
    size_t start = index_ - 1;
    ReadChar('-');

    char first_char = current_;
    if (!ReadDigit()) {
        std::stringstream ss;
        ss << "Excepted digit character, but got " << current_;
        throw JsonException(ss.str(), line_, column_);
    }
    if (first_char != '0') {
        while (!IsEOF() && ReadDigit()) {
        }
    }

    ReadFraction();
    ReadExponent();

    size_t count = index_ - start;
    std::string value_str = buffer_.substr(start, count);
    try {
        double value = std::stod(value_str);
        return new JsonNumber(value);
    } catch (...) {
        std::stringstream ss;
        ss << "Invalid number: " << value_str;
        throw JsonException(ss.str(), line_, column_);
    }
}

bool JsonParser::ReadDigit() {
    if (!(current_ >= '0' && current_ <= '9')) {
        return false;
    }

    Read();
    return true;
}

bool JsonParser::ReadFraction() {
    if (!ReadChar('.')) {
        return false;
    }
    if (!ReadDigit()) {
        std::stringstream ss;
        ss << "Expected digit character in fraction part but got " << current_;
        throw JsonException(ss.str(), line_, column_);
    }

    while (!IsEOF() && ReadDigit()) {
    }

    return true;
}

bool JsonParser::ReadExponent() {
    if (!ReadChar('e') && !ReadChar('E')) {
        return false;
    }
    if (!ReadChar('+')) {
        ReadChar('-');
    }
    if (!ReadDigit()) {
        std::stringstream ss;
        ss << "Expected digit character in exponent part but got " << current_;
        throw JsonException(ss.str(), line_, column_);
    }
    while (!IsEOF() && ReadDigit()) {
    }
    return true;
}

JsonValue *JsonParser::ReadString() {
    std::string value = ReadStringInternal();
    return new JsonString(value);
}

void JsonParser::ReadEscape(std::string &value) {
    Read();

    switch (current_) {
    case '"':
    case '/':
    case '\\':
        value.push_back(current_);
        break;
    case 'b':
        value.push_back('\b');
        break;
    case 'f':
        value.push_back('\f');
        break;
    case 'n':
        value.push_back('\n');
        break;
    case 'r':
        value.push_back('\r');
        break;
    case 't':
        value.push_back('\t');
        break;
    case 'u': {
        std::string unicode_chars;
        for (int i = 0; i < 4; ++i) {
            Read();
            if (!IsHexDigit(current_)) {
                std::stringstream ss;
                ss << "Got unexpected character in Unicode Character: " << current_;
                throw JsonException(ss.str(), line_, column_);
            }

            unicode_chars.push_back(current_);
        }

        int unicode_num = std::stoi(unicode_chars, nullptr, 16);
        value.append(std::to_string(unicode_num));
        break;
    }
    default: {
        std::stringstream ss;
        ss << "Got unexpected escaped character: " << current_;
        throw JsonException(ss.str(), line_, column_);
    }
    }

    Read();
}

JsonValue *JsonParser::ReadArray() {
    Read();
    SkipWhiteSpace();

    std::vector<JsonValue *> value;
    if (ReadChar(']')) {
        return new JsonArray(value);
    }

    do {
        SkipWhiteSpace();
        value.push_back(ReadValue());
        SkipWhiteSpace();
    } while (ReadChar(','));

    if (!ReadChar(']')) {
        throw JsonException("Found unclosed list", line_, column_);
    }

    return new JsonArray(value);
}

JsonValue *JsonParser::ReadObject() {
    Read();
    SkipWhiteSpace();

    std::map<std::string, JsonValue *> value;
    if (ReadChar('}')) {
        return new JsonObject(value);
    }

    do {
        SkipWhiteSpace();
        std::string property_name = ReadPropertyName();
        SkipWhiteSpace();
        if (!ReadChar(':')) {
            std::stringstream ss;
            ss << "Expect ':' but got " << current_;
            throw JsonException(ss.str(), line_, column_);
        }

        SkipWhiteSpace();
        JsonValue *property_value = ReadValue();
        SkipWhiteSpace();

        value[property_name] = property_value;
    } while (ReadChar(','));

    if (!ReadChar('}')) {
        throw JsonException("Found unclosed object", line_, column_);
    }

    return new JsonObject(value);
}

void JsonParser::SkipWhiteSpace() {
    while (!IsEOF() && IsWhiteSpace()) {
        Read();
    }
}

bool JsonParser::IsWhiteSpace() const noexcept {
    return current_ == ' ' || current_ == '\t' || current_ == '\n' || current_ == '\r';
}

std::string JsonParser::ReadPropertyName() {
    if (current_ != '"') {
        throw JsonException("Expect property name", line_, column_);
    }

    return ReadStringInternal();
}
std::string JsonParser::ReadStringInternal() {
    Read();

    std::string value;
    while (current_ != '"') {
        if (current_ == '\\') {
            ReadEscape(value);
        } else if (current_ < 0x20) {
            std::stringstream ss;
            ss << "Invalid character in string: code=" << int(current_);
            throw JsonException(ss.str(), line_, column_);
        } else {
            value.push_back(current_);
            Read();
        }
    }

    Read();
    return value;
}
bool JsonParser::IsEOF() const noexcept {
    return index_ >= limit_;
}
