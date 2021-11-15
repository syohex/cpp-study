#include <iostream>

struct Foo {
    explicit Foo(int x) : x_(x) {
    }

    explicit operator int() const {
        return x_;
    }
    explicit operator const int *() const {
        return &x_;
    }

    int x_;
};

int main() {
    Foo f(42);
    std::cout << static_cast<int>(f) << std::endl;
    std::cout << *static_cast<const int *>(f) << std::endl;
    return 0;
}
