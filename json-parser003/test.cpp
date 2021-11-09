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

} // namespace

int main() {
    TestNull();
    TestBoolean();
    return 0;
}