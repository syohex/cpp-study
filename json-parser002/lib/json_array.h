#pragma once

#include <vector>

#include "json_value.h"

class JsonArray : public JsonValue {
  public:
    explicit JsonArray(std::vector<JsonValue *> value);
    ~JsonArray() override;

    [[nodiscard]] bool IsArray() const noexcept override;
    [[nodiscard]] bool operator==(const JsonValue &value) const noexcept;

  private:
    std::vector<JsonValue *> value_;
};
