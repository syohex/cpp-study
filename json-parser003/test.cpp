#include <cassert>

#include "json_parser.h"

namespace {

void TestNull() {
    JsonValue v;
    std::string error = ParseJson("null", v);
    assert(error.empty());
    assert(v.IsNull());
}

void TestBoolean() {
    struct TestData {
        std::string input;
        bool expected;
    } test_data[] = {
        {"true", true},
        {"false", false},
    };

    for (const auto &t : test_data) {
        JsonValue v;
        std::string error = ParseJson(t.input, v);
        assert(error.empty());
        assert(v.IsBoolean());
        assert(v.Get<bool>() == t.expected);
    }
}

void TestNumber() {
    struct TestData {
        std::string input;
        double expected;
    } test_data[] = {
        {"0.5", 0.5},
        {"123.0", 123},
    };

    for (const auto &t : test_data) {
        JsonValue v;
        std::string error = ParseJson(t.input, v);
        assert(error.empty());
        assert(v.IsNumber());
        assert(v.Get<double>() == t.expected);
    }
}

void TestInteger() {
    struct TestData {
        std::string input;
        std::int64_t expected;
    } test_data[] = {
        {"42", 42},
        {"52719", 52719},
    };

    for (const auto &t : test_data) {
        JsonValue v;
        std::string error = ParseJson(t.input, v);
        assert(error.empty());
        assert(v.IsInteger());
        assert(v.Get<std::int64_t>() == t.expected);
    }
}

void TestString() {
    struct TestData {
        std::string input;
        std::string expected;
    } test_data[] = {
        {"\"hello world\"", "hello world"},
        {"\"a\\n\\b\"", "a\n\b"},
    };

    for (const auto &t : test_data) {
        JsonValue v;
        std::string error = ParseJson(t.input, v);
        assert(error.empty());
        assert(v.IsString());
        assert(v.Get<std::string>() == t.expected);
    }
}

void TestArray() {
    struct TestData {
        std::string input;
        JsonArray expected;
    } test_data[] = {
        {"[1.0,true,null]", {JsonValue(1.0), JsonValue(true), JsonValue()}},
        {"[[\"hello\"]]", {JsonValue(JsonArray{JsonValue("hello")})}},
    };

    for (const auto &t : test_data) {
        JsonValue v;
        std::string error = ParseJson(t.input, v);
        assert(error.empty());
        assert(v.IsArray());
        assert(v.Get<JsonArray>() == t.expected);
    }
}

void TestObject() {
    JsonObject expected{
        {"name", JsonValue("tom")},
        {"age", JsonValue(99.0)},
    };

    struct TestData {
        std::string input;
        JsonObject expected;
    } test_data[] = {
        {R"({"name": "tom", "age": 99.0})", expected},
    };

    for (const auto &t : test_data) {
        JsonValue v;
        std::string error = ParseJson(t.input, v);
        assert(error.empty());
        assert(v.IsObject());
        assert(v.Get<JsonObject>() == t.expected);
    }
}

} // namespace

int main() {
    TestNull();
    TestBoolean();
    TestNumber();
    TestInteger();
    TestString();
    TestArray();
    TestObject();

    return 0;
}