#pragma once

#include <vector>
#include <map>
#include <string>
#include <exception>

class CommandLineError : public std::exception {
public:
    explicit CommandLineError(std::string message);

    const char *what() const noexcept override;

private:
    std::string message_;
};

class OptionBase {
public:
    virtual ~OptionBase() = default;
};

class CommandLineParser {
public:
    ~CommandLineParser();

    void Add(const std::string &long_name, char short_name, const std::string &description);

private:
    std::map<std::string, OptionBase *> options_;
    std::vector <std::string> option_order_;
};