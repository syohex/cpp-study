#include <cassert>

#include "json_parser.h"

namespace {

void TestNull() {
    JsonValue v;
    std::string error = ParseJson("null", v);
    assert(error.empty());
    assert(v.IsNull());
}

} // namespace

int main() {
    TestNull();
    return 0;
}