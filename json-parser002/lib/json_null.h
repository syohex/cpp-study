#pragma once

#include <cstddef>

#include "json_value.h"

class JsonNull : public JsonValue {
  public:
    JsonNull();

    [[nodiscard]] bool IsNull() const noexcept override;
    bool operator==(const JsonValue &value) const noexcept;

    std::nullptr_t Value() const noexcept;
};