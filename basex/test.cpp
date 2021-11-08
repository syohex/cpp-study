#include <cassert>
#include <cstdio>

#include "basex.h"

int main() {
    {
        std::string alphabet("0123456789abcdef");
        struct TestData {
            std::string input;
            std::string expected;
        } test_data[] = {
            {"", ""},
            {"A", "41"},
            {"ABCDEF", "414243444546"},
        };

        for (const auto &t : test_data) {
            std::vector<std::uint8_t> source(t.input.begin(), t.input.end());
            BaseX b;
            assert(b.Init(alphabet) == BaseXError::kOk);
            assert(b.Encode(source) == t.expected);
        }
    }
    {
        std::string alphabet("0123456789abcdef");
        struct TestData {
            std::string input;
            std::string expected;
        } test_data[] = {
            {"", ""},
            {"41", "A"},
            {"414243444546", "ABCDEF"},
        };

        for (const auto &t : test_data) {
            std::vector<std::uint8_t> source(t.input.begin(), t.input.end());
            std::vector<std::uint8_t> ret;
            BaseX b;
            assert(b.Init(alphabet) == BaseXError::kOk);
            assert(b.Decode(t.input, ret) == BaseXError::kOk);

            std::string tmp(ret.begin(), ret.end());
            assert(tmp == t.expected);
        }
    }
    return 0;
}