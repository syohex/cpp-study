#pragma once

enum class JsonType {
    kObject,
    kArray,
    kNumber,
    kString,
    kBoolean,
    kNull,
};

class JsonValue {
  public:
    virtual ~JsonValue() = default;

    virtual bool IsObject() const noexcept;
    virtual bool IsArray() const noexcept;
    virtual bool IsNumber() const noexcept;
    virtual bool IsString() const noexcept;
    virtual bool IsBoolean() const noexcept;
    virtual bool IsNull() const noexcept;

    [[nodiscard]] JsonType Type() const noexcept;

    virtual bool operator==(const JsonValue &other) const;

  protected:
    explicit JsonValue(JsonType type);

    JsonType type_;
};