#pragma once

#include <map>
#include <string>

#include "json_value.h"

class JsonObject : public JsonValue {
  public:
    explicit JsonObject(std::map<std::string, JsonValue *> value);
    ~JsonObject() override;

    [[nodiscard]] bool IsObject() const noexcept override;
    [[nodiscard]] bool operator==(const JsonValue &value) const noexcept;

  private:
    std::map<std::string, JsonValue *> value_;
};