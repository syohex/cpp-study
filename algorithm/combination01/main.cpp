#include <cstdio>
#include <string>
#include <vector>
#include <functional>

void combination(const std::string &input, size_t start, std::string acc, std::vector<std::string> &ret) {
    if (start >= input.size()) {
        if (!acc.empty()) {
            ret.push_back(acc);
        }
        return;
    }

    combination(input, start + 1, acc + input[start], ret);
    combination(input, start + 1, acc, ret);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s word\n", argv[0]);
        return 1;
    }

    std::vector<std::string> ret;
    std::string input(argv[1]);
    combination(input, 0, "", ret);

    for (const auto &s : ret) {
        printf("'%s'\n", s.c_str());
    }

    return 0;
}
