#include "basex.h"

#include <algorithm>

BaseXError BaseX::Init(const std::string &alphabet) {
    std::map<char, size_t> m;
    std::vector<char> v;
    for (size_t i = 0; i < alphabet.size(); ++i) {
        char c = alphabet[i];
        if (m.find(c) != m.end()) {
            return BaseXError::kInvalidInput;
        }

        m[c] = i;
        v.push_back(c);
    }

    base_ = alphabet.size();
    alphabet_table_ = std::move(m);
    alphabet_ = std::move(v);

    return BaseXError::kOk;
}

std::string BaseX::Encode(const std::vector<std::uint8_t> &source) const {
    if (source.empty()) {
        return "";
    }

    std::vector<std::uint8_t> digits{0};
    for (size_t i = 0; i < source.size(); ++i) {
        size_t carry = source[i];

        for (size_t j = 0; j < digits.size(); ++j) {
            carry += digits[j] << 8;
            digits[j] = carry % base_;
            carry = carry / base_;
        }

        while (carry > 0) {
            digits.push_back(carry % base_);
            carry /= base_;
        }
    }

    std::string ret;
    for (size_t i = 0; source[i] == 0 && i < source.size() - 1; ++i) {
        ret.push_back(alphabet_[0]);
    }

    for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
        ret.push_back(alphabet_[*it]);
    }

    return ret;
}

BaseXError BaseX::Decode(const std::string &source, std::vector<std::uint8_t> &data) const {
    if (source.empty()) {
        return BaseXError::kOk;
    }

    data = {0};
    for (size_t i = 0; i < source.size(); ++i) {
        char c = source[i];
        if (alphabet_table_.find(c) == alphabet_table_.end()) {
            return BaseXError::kNoBaseCharacter;
        }

        size_t carry = alphabet_table_.at(c);
        for (size_t j = 0; j < data.size(); ++j) {
            carry += data[j] * base_;
            data[j] = static_cast<std::uint8_t>(carry & 0xff);
            carry >>= 8;
        }

        while (carry > 0) {
            data.push_back(static_cast<std::uint8_t>(carry & 0xff));
            carry >>= 8;
        }
    }

    for (size_t i = 0; source[i] == alphabet_[0] && i < source.size() - 1; ++i) {
        data.push_back(0);
    }

    int len = data.size();
    for (int i = 0, j = len - 1; i < j; ++i, --j) {
        std::swap(data[i], data[j]);
    }

    return BaseXError::kOk;
}
