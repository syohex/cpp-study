#include "json.h"

#include <sstream>

namespace json {

std::string deparse(const JsonValue &value, const std::string &whitespace) {
    switch (value.type) {
    case JsonValueType::kString:
        return "\"" + value.string.value() + "\"";
    case JsonValueType::kBoolean:
        return value.boolean.value() ? "true" : "false";
    case JsonValueType::kNumber:
        return std::to_string(value.number.value());
    case JsonValueType::kNull:
        return "null";
    case JsonValueType::kArray: {
        std::stringstream ss;
        ss << "[\n";

        const auto &array = value.array.value();
        for (size_t i = 0; i < array.size(); ++i) {
            ss << whitespace << " " << deparse(array[i], whitespace + " ");
            if (i < array.size() - 1) {
                ss << ",";
            }
            ss << "\n";
        }

        ss << whitespace << "\n";
        return ss.str();
    }
    case JsonValueType::kObject: {
        std::stringstream ss;
        ss << "{"
           << "\n";

        int i = 0;
        const auto &object = value.object.value();
        for (const auto &[key, value] : object) {
            ss << whitespace << " "
               << "\"" << key << "\": " << deparse(value, whitespace + " ");

            if (i < object.size() - 1) {
                ss << ",";
            }

            ss << "\n";
            ++i;
        }

        ss << whitespace << "}";
        return ss.str();
    }
    }
}

} // namespace json