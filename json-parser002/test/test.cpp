#include <cassert>
#include <iostream>
#include <json_parser.h>
#include <json_array.h>
#include <json_boolean.h>
#include <json_null.h>
#include <json_object.h>
#include <json_number.h>
#include <json_string.h>

namespace {

void TestBoolean() {
    std::cout << "=== Boolean test start ===" << std::endl;

    {
        JsonParser parser;
        JsonValue *v = parser.Parse("true");
        assert(v->Type() == JsonType::kBoolean);

        auto *b = reinterpret_cast<JsonBoolean *>(v);
        assert(b->Value());
        delete v;
    }
    {
        JsonParser parser;
        JsonValue *v = parser.Parse("false");
        assert(v->Type() == JsonType::kBoolean);

        auto *b = reinterpret_cast<JsonBoolean *>(v);
        assert(!b->Value());
        delete v;
    }
    std::cout << "=== Boolean test finish ===" << std::endl;
}

void TestNull() {
    std::cout << "=== Null test start ===" << std::endl;

    {
        JsonParser parser;
        JsonValue *v = parser.Parse("null");
        assert(v->Type() == JsonType::kNull);

        auto *n = reinterpret_cast<JsonNull *>(v);
        assert(n->Value() == nullptr);
        delete v;
    }
    std::cout << "=== Null test finish ===" << std::endl;
}

void TestNumber() {
    std::cout << "=== Number test start ===" << std::endl;

    struct Data {
        std::string input;
        double expected;
    } test_data[] = {
        {"12345", 12345},
        {"-987", -987},
        {"0.5", 0.5},
    };

    for (const auto &test : test_data) {
        JsonParser parser;
        JsonValue *v = parser.Parse(test.input);
        assert(v->Type() == JsonType::kNumber);

        auto *n = reinterpret_cast<JsonNumber *>(v);
        assert(n->Value() == test.expected);
        delete v;
    }
    std::cout << "=== Number test finish ===" << std::endl;
}

void TestString() {
    std::cout << "=== String test start ===" << std::endl;

    struct Data {
        std::string input;
        std::string expected;
    } test_data[] = {
        {"\"hello world\"", "hello world"},
        {R"("\b\n\t\/\r\f")", "\b\n\t/\r\f"},
    };

    for (const auto &test : test_data) {
        JsonParser parser;
        JsonValue *v = parser.Parse(test.input);
        assert(v->Type() == JsonType::kString);

        auto *s = reinterpret_cast<JsonString *>(v);
        assert(s->Value() == test.expected);
        delete v;
    }
    std::cout << "=== String test finish ===" << std::endl;
}

void TestArray() {
    std::cout << "=== Array test start ===" << std::endl;

    std::vector<JsonValue *> vec;
    vec.push_back(new JsonNumber(123));
    vec.push_back(new JsonBoolean(true));
    vec.push_back(new JsonBoolean(false));
    vec.push_back(new JsonNull());
    vec.push_back(new JsonString("hello"));
    std::unique_ptr<JsonArray> expected(new JsonArray(vec));

    JsonParser parser;
    JsonValue *v = parser.Parse("[123, true,  false, null, \"hello\"]");
    assert(v->Type() == JsonType::kArray);

    auto *arr = reinterpret_cast<JsonArray *>(v);
    assert(*arr == *expected);
    delete v;

    std::cout << "=== Array test finish ===" << std::endl;
}

void TestObject() {
    std::cout << "=== Object test start ===" << std::endl;

    std::map<std::string, JsonValue *> obj;
    obj.insert({"name", new JsonString("Tom")});
    obj.insert({"age", new JsonNumber(88)});
    std::unique_ptr<JsonObject> expected(new JsonObject(obj));

    JsonParser parser;
    JsonValue *v = parser.Parse(R"({"name": "Tom", "age": 88})");
    assert(v->Type() == JsonType::kObject);

    auto *v_obj = reinterpret_cast<JsonObject *>(v);
    assert(*v_obj == *expected);
    delete v;

    std::cout << "=== Object test finish ===" << std::endl;
}

} // namespace

int main() {
    TestBoolean();
    TestNull();
    TestNumber();
    TestString();
    TestArray();
    TestObject();
    return 0;
}