#pragma once

#include <string>

#include "json_value.h"

class JsonString : public JsonValue {
  public:
    explicit JsonString(std::string value);

    [[nodiscard]] bool IsString() const noexcept override;
    bool operator==(const JsonValue &value) const noexcept;

    std::string Value() const noexcept;

  private:
    std::string value_;
};