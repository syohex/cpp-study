#pragma once

#include <string>

template <typename Iter>
class InputSource {
  public:
    InputSource(const Iter &current, const Iter &end) : current_(current), end_(end), consumed_(false), line_(1) {
    }

    int GetChar() {
        if (consumed_) {
            if (*current_ == '\n') {
                ++line_;
            }

            ++current_;
        }

        if (current_ == end_) {
            consumed_ = false;
            return END_OF_INPUT;
        }

        consumed_ = true;
        return *current_ & 0xff;
    }

    void UnGetChar() {
        consumed_ = false;
    }

    Iter Current() {
        if (consumed_) {
            consumed_ = false;
            ++current_;
        }

        return current_;
    }

    int Line() const noexcept {
        return line_;
    }

    void SkipWhiteSpace() {
        while (true) {
            int ch = GetChar();
            if (!(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')) {
                UnGetChar();
                break;
            }
        }
    }

    bool Expect(int expected) {
        SkipWhiteSpace();
        if (GetChar() != expected) {
            UnGetChar();
            return false;
        }

        return true;
    }

    bool Match(const std::string &pattern) {
        for (auto it = pattern.cbegin(); it != pattern.cend(); ++it) {
            if (GetChar() != static_cast<int>(*it)) {
                UnGetChar();
                return false;
            }
        }

        return true;
    }

    static constexpr int END_OF_INPUT = -1;

  private:
    Iter current_;
    Iter end_;
    bool consumed_;
    int line_;
};