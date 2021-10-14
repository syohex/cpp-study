#include <cassert>
#include <map>
#include <iterator>
#include <vector>
#include <string>

template <typename T>
class TrieNode {
  public:
    typedef typename T::value_type ElementT;
    typedef typename T::iterator IteratorT;
    typedef typename T::const_iterator ConstIteratorT;

    TrieNode() : end_(false) {
    }

    void Insert(ConstIteratorT it, const T &orig) {
        if (table_.find(*it) == table_.end()) {
            table_[*it] = new TrieNode<T>();
        }

        auto next = std::next(it, 1);
        if (next == orig.end()) {
            table_[*it]->end_ = true;
            return;
        }

        table_[*it]->Insert(next, orig);
    }

    bool Find(ConstIteratorT it, const T &orig) {
        if (table_.find(*it) == table_.end()) {
            return false;
        }

        auto next = std::next(it, 1);
        if (next == orig.end()) {
            return table_[*it]->end_;
        }

        return table_[*it]->Find(next, orig);
    }

  private:
    std::map<ElementT, TrieNode *> table_;
    bool end_;
};

template <typename T>
class Trie {
  public:
    Trie() : root_(new TrieNode<T>()) {
    }

    void Insert(const T &value) {
        root_->Insert(value.begin(), value);
    }

    bool Find(const T &value) const {
        return root_->Find(value.begin(), value);
    }

  private:
    TrieNode<T> *root_;
};

int main() {
    {
        Trie<std::string> t;
        t.Insert("app");
        t.Insert("apple");

        assert(t.Find("app"));
        assert(t.Find("apple"));
        assert(!t.Find("apps"));
    }
    {
        Trie<std::vector<int>> t;
        t.Insert({1, 2, 3});
        t.Insert({1});

        assert(t.Find({1}));
        assert(t.Find({1, 2, 3}));
        assert(!t.Find({1, 2}));
    }
    return 0;
}