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
            "[", "true", ",", "false", ",", "1234", ",", "foo", "]",
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

void test_parse() {
    {
        std::vector<std::string> input{"1", "true", "false", "\"foo\"", "null"};
        std::vector<JsonValue> expected{JsonValue(1.0), JsonValue(true), JsonValue(false), JsonValue(std::string("foo")),
                                        JsonValue()};
        for (size_t i = 0; i < input.size(); ++i) {
            auto [value, error] = parse(input[i]);
            assert(error.empty());
            assert(value == expected[i]);
        }
    }
    {
        auto [value, error] = parse("[1, 2, true, null, \"foo\"]");
        assert(error.empty());
        assert(value.type == JsonValueType::kArray);
        assert(value.array.value().size() == 5);

        std::vector<JsonValue> expected{JsonValue(1.0), JsonValue(2.0), JsonValue(true), JsonValue(),
                                        JsonValue(std::string("foo"))};
        for (size_t i = 0; i < 5; ++i) {
            assert(value.array.value()[i] == expected[i]);
        }
    }
    {
        auto [value, error] = parse("{\"name\": \"bob\", \"age\": 88}");
        assert(error.empty());
        assert(value.type == JsonValueType::kObject);
        const auto &object = value.object.value();
        assert(object.at("name") == JsonValue(std::string("bob")));
        assert(object.at("age") == JsonValue(88.0));
    }
}

} // namespace

int main() {
    test_lex();
    test_parse();

    std::cout << "OK" << std::endl;
    return 0;
}