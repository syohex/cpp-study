#pragma once

#include "json_value.h"

class JsonNumber : public JsonValue {
  public:
    explicit JsonNumber(double value);

    [[nodiscard]] bool IsNumber() const noexcept override;
    bool operator==(const JsonValue &value) const noexcept;

    double Value() const noexcept;

  private:
    double value_;
};