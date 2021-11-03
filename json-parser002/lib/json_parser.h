#pragma once

#include <string>

#include "json_value.h"

class JsonParser {
  public:
    JsonParser();
    JsonValue *Parse(const std::string &input);

  private:
    void Read();
    bool ReadChar(char ch);
    void ReadRequiredChar(char ch);
    [[nodiscard]] JsonValue *ReadValue();

    JsonValue *ReadNull();
    JsonValue *ReadTrue();
    JsonValue *ReadFalse();
    JsonValue *ReadNumber();
    JsonValue *ReadString();
    JsonValue *ReadArray();
    JsonValue *ReadObject();

    bool ReadDigit();
    bool ReadFraction();
    bool ReadExponent();
    void ReadEscape(std::string &value);
    bool IsEOF() const noexcept;

    void SkipWhiteSpace();
    [[nodiscard]] bool IsWhiteSpace() const noexcept;
    std::string ReadStringInternal();
    std::string ReadPropertyName();

    std::string buffer_;
    char current_;
    size_t index_;
    size_t limit_;
    int line_;
    int column_;
};