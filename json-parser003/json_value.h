#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

#define JSON_ASSERT(cond)                                                                                                          \
    do {                                                                                                                           \
        if (!(cond))                                                                                                               \
            throw std::runtime_error(#cond);                                                                                       \
    } while (0)

enum class JsonType {
    kNull,
    kBoolean,
    kNumber,
    kInteger,
    kString,
    kArray,
    kObject,
};

class JsonValue;
using JsonArray = std::vector<JsonValue>;
using JsonObject = std::map<std::string, JsonValue>;

class JsonValue {
  public:
    JsonValue();

    // null constructor
    explicit JsonValue(std::nullptr_t value);

    // boolean constructor
    explicit JsonValue(bool value);

    // number constructor
    explicit JsonValue(double value);
    explicit JsonValue(std::int64_t value);

    // string constructor
    explicit JsonValue(JsonType type);
    explicit JsonValue(const std::string &value);
    explicit JsonValue(std::string &&value);
    explicit JsonValue(const char *value);

    // array constructor
    explicit JsonValue(const JsonArray &value);
    explicit JsonValue(JsonArray &&value);

    // object constructor
    explicit JsonValue(const JsonObject &value);
    explicit JsonValue(JsonObject &&value);

    ~JsonValue();

    // Predicate
    bool IsNull() const noexcept;
    bool IsBoolean() const noexcept;
    bool IsNumber() const noexcept;
    bool IsInteger() const noexcept;
    bool IsArray() const noexcept;
    bool IsObject() const noexcept;

    template <typename T>
    bool Is() const noexcept;

    template <typename T>
    const T &Get() const;
    template <typename T>
    T &Get();

    template <typename T>
    void Set(const T &value);
    template <typename T>
    void Set(T &&value);

    JsonType Type() const noexcept;

  private:
    void Clear();

    JsonType type_;
    union {
        bool boolean_;
        double number_;
        std::int64_t int64_;
        std::string *string_;
        JsonArray *array_;
        JsonObject *object_;
    } u_;
};

#define IS(c_type, json_type)                                                                                                      \
    template <>                                                                                                                    \
    inline bool JsonValue::Is<c_type>() const noexcept {                                                                                    \
        return type_ == (json_type);                                                                                               \
    }

IS(std::nullptr_t, JsonType::kNull)
IS(bool, JsonType::kBoolean)
IS(std::int64_t, JsonType::kInteger)
IS(std::string, JsonType::kString)
IS(JsonArray, JsonType::kArray)
IS(JsonObject, JsonType::kObject)

#undef IS

#define GET(c_type, var)                                                                                                           \
    template <>                                                                                                                    \
    inline const c_type &JsonValue::Get<c_type>() const {                                                                          \
        JSON_ASSERT(Is<c_type>());                                                                                                 \
        return (var);                                                                                                              \
    }                                                                                                                              \
    template <>                                                                                                                    \
    inline c_type &JsonValue::Get<c_type>() {                                                                                      \
        JSON_ASSERT(Is<c_type>());                                                                                                 \
        return (var);                                                                                                              \
    }

GET(bool, u_.boolean_)
GET(std::int64_t, u_.int64_)
GET(std::string, *u_.string_)
GET(JsonArray, *u_.array_)
GET(JsonObject, *u_.object_)

#undef GET

// for number
template <>
inline bool JsonValue::Is<double>() const noexcept {
    return type_ == JsonType::kNumber || type_ == JsonType::kInteger;
}

template <>
inline const double &JsonValue::Get<double>() const {
    JSON_ASSERT(Is<double>());
    if (type_ == JsonType::kNumber) {
        return u_.number_;
    }

    auto *self = const_cast<JsonValue *>(this);
    self->type_ = JsonType::kNumber;
    self->u_.number_ = self->u_.int64_;
    return self->u_.number_;
}

template <>
inline double &JsonValue::Get<double>() {
    JSON_ASSERT(Is<double>());
    if (type_ == JsonType::kNumber) {
        return u_.number_;
    }

    auto *self = const_cast<JsonValue *>(this);
    self->type_ = JsonType::kNumber;
    self->u_.number_ = self->u_.int64_;
    return self->u_.number_;
}

#define SET(c_type, json_type, setter)                                                                                             \
    template <>                                                                                                                    \
    inline void JsonValue::Set<c_type>(const c_type &value) {                                                                      \
        Clear();                                                                                                                   \
        type_ = (json_type);                                                                                                       \
        setter;                                                                                                                    \
    }

SET(bool, JsonType::kBoolean, u_.boolean_ = value)
SET(double, JsonType::kNumber, u_.number_ = value)
SET(std::int64_t, JsonType::kInteger, u_.int64_ = value)
SET(std::string, JsonType::kString, u_.string_ = new std::string(value))
SET(JsonArray, JsonType::kArray, u_.array_ = new JsonArray(value))
SET(JsonObject, JsonType::kObject, u_.object_ = new JsonObject(value))

#undef SET

#define RVALUE_SET(c_type, json_type, setter)                                                                                      \
    template <>                                                                                                                    \
    inline void JsonValue::Set<c_type>(c_type && value) {                                                                          \
        Clear();                                                                                                                   \
        type_ = (json_type);                                                                                                       \
        setter;                                                                                                                    \
    }

RVALUE_SET(std::string, JsonType::kString, u_.string_ = new std::string(std::move(value)))
RVALUE_SET(JsonArray, JsonType::kArray, u_.array_ = new JsonArray(std::move(value)))
RVALUE_SET(JsonObject, JsonType::kObject, u_.object_ = new JsonObject(std::move(value)))

#undef RVALUE_SET