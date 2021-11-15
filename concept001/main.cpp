#include <iostream>

template <typename T>
concept Foo = requires(T t) {
    t.Func1();
    t.Func2();
};

template <typename T>
requires Foo<T>
int func(const T &t) {
    return t.Func1() + t.Func2();
}

struct Test1 {
    int Func1() const {
        return 1;
    }
    int Func2() const {
        return 2;
    }
};

struct Test2 {
    int Func1() const {
        return 1;
    }
    int Func3() const {
        return 3;
    }
};

int main() {
    auto v1 = func(Test1{});
    // auto v2 = func(Test2{}); // compile error
    std::cout << v1 << std::endl;
    return 0;
}