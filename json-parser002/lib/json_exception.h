#pragma once

#include <exception>
#include <string>

class JsonException : public std::exception {
  public:
    explicit JsonException(const std::string &message, int line, int column);

    [[nodiscard]] const char *what() const noexcept override;

  private:
    std::string message_;
};
