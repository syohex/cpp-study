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

} // namespace

int main() {
    TestNull();
    TestBoolean();
    TestNumber();
    TestInteger();
    TestString();
    return 0;
}