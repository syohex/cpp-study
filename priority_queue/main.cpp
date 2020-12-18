#include <cassert>
#include <cstdio>
#include <queue>
#include <string>
#include <map>

namespace {

void Simple() {
    std::priority_queue<int> q;

    for (auto n : {9, 8, 2, 3, 4, 5}) {
        q.push(n);
    }

    printf("[");
    while (!q.empty()) {
        int n = q.top();
        printf(" %d ", n);
        q.pop();
    }
    printf("]\n");
}

void Greater() {
    std::priority_queue<int, std::vector<int>, std::greater<int>> q;

    for (auto n : {9, 8, 2, 3, 4, 5}) {
        q.push(n);
    }

    printf("[");
    while (!q.empty()) {
        int n = q.top();
        printf(" %d ", n);
        q.pop();
    }
    printf("]\n");
}

void Custom() {
    struct CharInfo {
        char c;
        int count;
    };

    std::string s("oijifojawoijfeiajiojdksakdfaslkdfjaksldjfioajfoiawjeoiajklm");
    std::map<char, int> m;
    for (char c : s) {
        ++m[c];
    }

    const auto cmp = [](const CharInfo &a, const CharInfo &b) { return a.count < b.count; };

    std::priority_queue<CharInfo, std::vector<CharInfo>, decltype(cmp)> q(cmp);
    for (const auto &it : m) {
        q.emplace(CharInfo{it.first, it.second});
    }

    printf("[");
    while (!q.empty()) {
        auto &info = q.top();
        printf(" %c:%d ", info.c, info.count);
        q.pop();
    }
    printf("]\n");
}

} // namespace

int main() {
    Simple();
    Greater();
    Custom();
    return 0;
}
