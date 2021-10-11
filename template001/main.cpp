#include <cassert>
#include <string>
#include <vector>
#include <iterator>

template <typename T>
struct Sample {
    typedef typename T::value_type ElementT;
    typedef typename T::iterator IterT;

    explicit Sample(T t) : value_(std::move(t)), it_(value_.begin()), end_(value_.end()) {
    }

    ElementT next() {
        ElementT ret = *it_;
        std::advance(it_, 1);
        return ret;
    }

    bool is_end() const {
        return it_ == end_;
    }

    T value_;
    IterT it_;
    IterT end_;
};

int main() {
    {
        Sample<std::string> s("melon");
        assert(!s.is_end());
        assert(s.next() == 'm');
        assert(s.next() == 'e');
        assert(s.next() == 'l');
        assert(s.next() == 'o');
        assert(s.next() == 'n');
        assert(s.is_end());
    }
    {
        Sample<std::vector<long>> s(std::vector<long>{1, 2, 3});
        assert(!s.is_end());
        assert(s.next() == 1);
        assert(s.next() == 2);
        assert(s.next() == 3);
        assert(s.is_end());
    }
    return 0;
}
