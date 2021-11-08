#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstddef>

enum class BaseXError {
    kOk,
    kInvalidInput,
    kNoBaseCharacter,
};

class BaseX {
  public:
    BaseXError Init(const std::string &alphabet);

    std::string Encode(const std::vector<std::uint8_t> &source) const;
    BaseXError Decode(const std::string &source, std::vector<std::uint8_t> &data) const;

  private:
    size_t base_;
    std::map<char, size_t> alphabet_table_;
    std::vector<char> alphabet_;
};