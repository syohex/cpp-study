#include <iostream>

int sum() {
    return 0;
}

template <typename T, typename... Args>
int sum(T n, Args... args) {
    return n + sum(args...);
}

int main() {
    int ret = sum(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    std::cout << "sum=" << ret << std::endl;
    return 0;
}