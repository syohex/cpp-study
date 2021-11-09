#pragma once

#include <cinttypes>
#include <errno.h>
#include <limits>
#include <sstream>

#include "json_value.h"
#include "input_source.h"

// RFC 8259 secion 7 Strings
inline bool _IsControlCharacter(int ch) {
    return ch >= 0 && ch < ' ';
}

template <typename Iter>
inline int _ParseQuadHex(InputSource<Iter> &in) {
    int unicode_char = 0;

    for (int i = 0; i < 4; ++i) {
        int hex = in.GetChar();
        if (hex == InputSource<Iter>::END_OF_INPUT) {
            return -1;
        }

        if (hex >= '0' && hex <= '9') {
            hex -= '0';
        } else if (hex >= 'A' && hex <= 'F') {
            hex = hex - 'A' + 0xa;
        } else if (hex >= 'a' && hex <= 'f') {
            hex = hex - 'a' + 0xa;
        } else {
            in.UnGetChar();
            return -1;
        }

        unicode_char = (unicode_char << 4) + hex;
    }

    return unicode_char;
}

template <typename String, typename Iter>
inline bool _ParseCodePoint(String &out, InputSource<Iter> &in) {
    int unicode_char = _ParseQuadHex(in);
    if (unicode_char == -1) {
        return false;
    }

    // Reference https://en.wikipedia.org/wiki/UTF-16#Code_points_from_U+010000_to_U+10FFFF
    // surrogate pair
    if (unicode_char >= 0xd800 && unicode_char <= 0xdfff) {
        if (unicode_char >= 0xdc00) {
            // low surrogate appeared
            return false;
        }

        // get next one
        if (in.GetChar() != '\\' || in.GetChar() != 'u') {
            in.UnGetChar();
            return false;
        }

        int second = _ParseQuadHex(in);
        if (!(second >= 0xdc00 && second <= 0xdfff)) {
            return false;
        }

        unicode_char = ((unicode_char - 0xd800) << 10) | ((second - 0xdc00));
        unicode_char += 0x10000;
    }

    // Code Point to UTF-8 character
    if (unicode_char < 0x80) {
        out.push_back(static_cast<char>(unicode_char));
    } else if (unicode_char < 0x0800) {
        char b1 = ((unicode_char >> 6) & 0x1f) | 0xc0;
        char b2 = (unicode_char & 0x3f) | 0x80;
        out.push_back(b1);
        out.push_back(b2);
    } else if (unicode_char < 0x10000) {
        char b1 = ((unicode_char >> 12) & 0xf) | 0xe0;
        char b2 = ((unicode_char >> 6) & 0x3f) | 0x80;
        char b3 = (unicode_char & 0x3f) | 0x80;
        out.push_back(b1);
        out.push_back(b2);
        out.push_back(b3);
    } else {
        char b1 = ((unicode_char >> 18) & 0x7) | 0xf0;
        char b2 = ((unicode_char >> 12) & 0x3f) | 0x80;
        char b3 = ((unicode_char >> 6) & 0x3f) | 0x80;
        char b4 = (unicode_char & 0x3f) | 0x80;
        out.push_back(b1);
        out.push_back(b2);
        out.push_back(b3);
        out.push_back(b4);
    }

    return true;
}

template <typename Iter>
inline bool _ParseString(std::string &out, InputSource<Iter> &in) {
    while (true) {
        int ch = in.GetChar();
        if (_IsControlCharacter(ch) || ch == InputSource<Iter>::END_OF_INPUT) {
            in.UnGetChar();
            return false;
        }

        if (ch == '"') {
            return true;
        }

        if (ch == '\\') {
            if ((ch = in.GetChar()) == InputSource<Iter>::END_OF_INPUT) {
                return false;
            }

            switch (ch) {
            case '"':
                out.push_back('"');
                break;
            case '\\':
                out.push_back('\\');
                break;
            case '/':
                out.push_back('/');
                break;
            case 'b':
                out.push_back('\b');
                break;
            case 'f':
                out.push_back('\f');
                break;
            case 'n':
                out.push_back('\n');
                break;
            case 'r':
                out.push_back('\r');
                break;
            case 't':
                out.push_back('\t');
                break;
            case 'u':
                if (!_ParseCodePoint(out, in)) {
                    return false;
                }
                break;
            default:
                return false;
            }
        } else {
            out.push_back(static_cast<char>(ch));
        }
    }

    return false;
}

class ParseContext;
template <typename Context, typename Iter>
inline bool _ParseArray(Context &context, InputSource<Iter> &in) {
    if (!context.ParseArrayStart()) {
        return false;
    }

    size_t index = 0;
    if (in.Expect(']')) {
        return context.ParseArrayStop();
    }

    do {
        if (!context.ParseArrayItem(in, index)) {
            return false;
        }

        ++index;
    } while (in.Expect(','));

    if (in.Expect(']')) {
        return context.ParseArrayStop();
    }

    return false;
}

template <typename Context, typename Iter>
inline bool _ParseObject(Context &context, InputSource<Iter> &in) {
    if (!context.ParseObjectStart()) {
        return false;
    }

    if (in.Expect('}')) {
        return context.ParseObjectStop();
    }

    do {
        std::string key;
        if (!in.Expect('"')) {
            return false;
        }
        if (!_ParseString(key, in)) {
            return false;
        }
        if (!in.Expect(':')) {
            return false;
        }

        if (!context.ParseObjectItem(in, key)) {
            return false;
        }
    } while (in.Expect(','));

    if (in.Expect('}')) {
        return context.ParseObjectStop();
    }

    return false;
}

template <typename Iter>
inline std::string _ParseNumber(InputSource<Iter> &in) {
    std::string num_string;
    while (true) {
        int ch = in.GetChar();
        if ((ch >= '0' && ch <= '9') || ch == '+' || ch == '-' || ch == 'e' || ch == 'E') {
            num_string.push_back(static_cast<char>(ch));
        } else if (ch == '.') {
            num_string.push_back('.');
        } else {
            in.UnGetChar();
            break;
        }
    }

    return num_string;
}

bool _IsValidInt64(std::intmax_t value, const std::string &value_str, char *endp) {
    if (!(value >= std::numeric_limits<std::int64_t>::min() && value <= std::numeric_limits<std::int64_t>::max())) {
        return false;
    }

    const char *limit = value_str.c_str() + value_str.size();
    return endp == limit;
}

template <typename Context, typename Iter>
inline bool _Parse(Context &context, InputSource<Iter> &in) {
    in.SkipWhiteSpace();

    int ch = in.GetChar();
    switch (ch) {
    case 'n':
        if (in.Match("ull") && context.SetNull()) {
            return true;
        }
        return false;
    case 't':
        if (in.Match("rue") && context.SetBool(true)) {
            return true;
        }
        return false;
    case 'f':
        if (in.Match("alse") && context.SetBool(false)) {
            return true;
        }
        return false;
    case '"':
        return context.ParseString(in);
    case '[':
        return _ParseArray(context, in);
    case '{':
        return _ParseObject(context, in);
    default:
        if ((ch >= '0' && ch <= '9') || ch == '-') {
            in.UnGetChar();

            std::string num_string = _ParseNumber(in);
            if (num_string.empty()) {
                return false;
            }

            errno = 0;
            char *endp;
            std::intmax_t ival = std::strtoimax(num_string.c_str(), &endp, 10);
            if (errno == 0 && _IsValidInt64(ival, num_string, endp)) {
                context.SetInt64(ival);
                return true;
            }

            double dval = std::strtod(num_string.c_str(), &endp);
            if (endp == num_string.c_str() + num_string.size()) {
                context.SetNumber(dval);
                return true;
            }

            return false;
        }

        break;
    }

    in.UnGetChar();
    return false;
}

class ParseContext {
  public:
    explicit ParseContext(JsonValue *value, size_t depth = DEFAULT_MAX_DEPTH) : value_(value), depth_(depth) {
    }

    bool SetNull() {
        *value_ = JsonValue();
        return true;
    }

    bool SetBool(bool value) {
        *value_ = JsonValue(value);
        return true;
    }

    bool SetNumber(double value) {
        *value_ = JsonValue(value);
        return true;
    }
    bool SetInt64(std::int64_t value) {
        *value_ = JsonValue(value);
        return true;
    }

    template <typename Iter>
    bool ParseString(InputSource<Iter> &in) {
        *value_ = JsonValue(JsonType::kString);

        std::string &str_value = value_->Get<std::string>();
        return _ParseString(str_value, in);
    }

    bool ParseArrayStart() {
        if (depth_ == 0) {
            return false;
        }

        --depth_;
        *value_ = JsonValue(JsonType::kArray);
        return true;
    }

    bool ParseArrayStop() {
        ++depth_;
        return true;
    }

    template <typename Iter>
    bool ParseArrayItem(InputSource<Iter> &in, size_t index) {
        JsonArray &array_value = value_->Get<JsonArray>();
        array_value.push_back(JsonValue());

        ParseContext context(&array_value.back(), depth_);
        return _Parse(context, in);
    }

    bool ParseObjectStart() {
        if (depth_ == 0) {
            return false;
        }

        *value_ = JsonValue(JsonType::kObject);
        return true;
    }

    template <typename Iter>
    bool ParseObjectItem(InputSource<Iter> &in, const std::string &key) {
        JsonObject &object_value = value_->Get<JsonObject>();
        ParseContext context(&object_value[key], depth_);
        return _Parse(context, in);
    }

    bool ParseObjectStop() {
        --depth_;
        return true;
    }

  private:
    static constexpr size_t DEFAULT_MAX_DEPTH = 100;

    JsonValue *value_;
    size_t depth_;
};

template <typename Iter>
inline Iter _Parse(ParseContext &context, const Iter &begin, const Iter &end, std::string *error) {
    InputSource<Iter> in(begin, end);

    bool ret = _Parse(context, in);
    if (!ret && error != nullptr) {
        std::stringstream ss;
        ss << "syntax error at line " << in.Line() << " near: ";
        *error = ss.str();

        while (true) {
            int ch = in.GetChar();
            if (ch == InputSource<Iter>::END_OF_INPUT || ch == '\n') {
                break;
            }

            if (!_IsControlCharacter(ch)) {
                error->push_back(static_cast<int>(ch));
            }
        }
    }

    return in.Current();
}

template <typename Iter>
Iter ParseJson(const Iter &begin, const Iter &end, JsonValue &value, std::string *error) {
    ParseContext context(&value);
    return _Parse(context, begin, end, error);
}

inline std::string ParseJson(const std::string &input, JsonValue &value) {
    std::string error;
    ParseJson(input.begin(), input.end(), value, &error);
    return error;
}
