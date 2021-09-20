#include <cassert>
#include <iostream>

#include "json.h"

namespace {

using namespace json;

void test_lex() {
    {
        struct {
            std::string input;
            JsonTokenType type;
            std::string expected;
        } input[] = {
            {"12345", JsonTokenType::kNumber}, {"true", JsonTokenType::kBoolean},          {"false", JsonTokenType::kBoolean},
            {"null", JsonTokenType::kNull},    {"\"foo\"", JsonTokenType::kString, "foo"},
        };

        for (const auto &d : input) {
            auto [tokens, error] = lex(d.input);
            assert(error.empty());
            assert(tokens.size() == 1);
            assert(tokens[0].type == d.type);

            if (d.expected.empty()) {
                assert(tokens[0].value == d.input);
            } else {
                assert(tokens[0].value == d.expected);
            }
        }
    }
    {
        auto [tokens, error] = lex("[true, false, 1234, \"foo\"]");
        assert(error.empty());
        assert(tokens.size() == 9);

        JsonTokenType types[9] = {
            JsonTokenType::kSyntax,  JsonTokenType::kBoolean, JsonTokenType::kSyntax,
            JsonTokenType::kBoolean, JsonTokenType::kSyntax,  JsonTokenType::kNumber,
            JsonTokenType::kSyntax,  JsonTokenType::kString,  JsonTokenType::kSyntax,
        };
        std::string values[9] = {
            "[",
            "true",
            ",",
            "false",
            ",",
            "1234",
            ",",
            "foo",
            "]",
        };

        for (size_t i = 0; i < 9; ++i) {
            assert(tokens[i].type == types[i]);
            assert(tokens[i].value == values[i]);
        }
    }
    {
        auto [tokens, error] = lex("{\"foo\": 1}");
        assert(error.empty());
        assert(tokens.size() == 5);

        JsonTokenType types[5] = {
            JsonTokenType::kSyntax, JsonTokenType::kString, JsonTokenType::kSyntax, JsonTokenType::kNumber, JsonTokenType::kSyntax,
        };
        std::string values[5] = {
            "{", "foo", ":", "1", "}",
        };

        for (size_t i = 0; i < 5; ++i) {
            assert(tokens[i].type == types[i]);
            assert(tokens[i].value == values[i]);
        }
    }
}

} // namespace

int main() {
    test_lex();

    std::cout << "OK" << std::endl;
    return 0;
}