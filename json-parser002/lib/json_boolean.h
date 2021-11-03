#pragma once

#include "json_value.h"

class JsonBoolean : public JsonValue {
  public:
    explicit JsonBoolean(bool value);

    [[nodiscard]] bool IsBoolean() const noexcept override;
    bool operator==(const JsonValue &value) const noexcept;

    bool Value() const noexcept;

  private:
    bool value_;
};