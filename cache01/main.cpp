#include <cassert>
#include <string>
#include <mutex>
#include <list>
#include <chrono>
#include <optional>
#include <vector>
#include <utility>

template <typename K, typename V>
class CacheItem {
  public:
    explicit CacheItem(const K &key, const V &value, std::chrono::seconds expiration)
        : key_(key), value_(value), expiration_(expiration) {
    }

    const K &Key() const noexcept {
        return key_;
    }

    const V &Value() const noexcept {
        return value_;
    }

    std::chrono::seconds Expiration() const noexcept {
        return expiration_;
    }

  private:
    K key_;
    V value_;
    std::chrono::seconds expiration_;
};

enum class CacheError {
    kOK,
    kKeyAlreadyExists,
    kNotFound,
};

template <typename K, typename V>
class Cache {
  public:
    explicit Cache(size_t capacity) : capacity_(capacity) {
        assert(capacity != 0);
    }

    CacheError Add(const K &key, const V &value, std::chrono::seconds expiration) {
        auto it = Find(key);
        if (it != list_.end()) {
            return CacheError::kKeyAlreadyExists;
        }

        auto exp_time = NowSeconds() + expiration;
        auto item = CacheItem(key, value, exp_time);

        std::lock_guard<std::mutex> scoped_lock(mutex_);
        if (list_.size() == capacity_) {
            const auto &item = LRUItem();
            Delete(item.Key());
        }

        list_.push_front(std::move(item));
        return CacheError::kOK;
    }

    std::optional<V> Get(const K &key) {
        if (list_.empty()) {
            return std::nullopt;
        }

        std::lock_guard<std::mutex> scoped_lock(mutex_);
        auto it = Find(key);
        if (it == list_.end()) {
            return std::nullopt;
        }

        CacheItem item = *it;
        list_.erase(it);
        list_.push_front(std::move(item));
        return item.Value();
    }

    CacheError Remove(const K &key) {
        if (list_.empty()) {
            return CacheError::kNotFound;
        }

        std::lock_guard<std::mutex> scoped_lock(mutex_);
        Delete(key);
        return CacheError::kOK;
    }

    bool Contains(const K &key) const {
        if (list_.empty()) {
            return false;
        }

        std::lock_guard<std::mutex> scoped_lock(mutex_);
        return Find(key) != list_.end();
    }

    std::vector<K> Keys() const {
        std::vector<K> ret;

        {
            std::lock_guard<std::mutex> scoped_lock(mutex_);
            for (const auto &it : list_) {
                ret.push_back(it.Key());
            }
        }

        return ret;
    }

    std::optional<V> Peek(const K &key) {
        if (list_.empty()) {
            return std::nullopt;
        }

        std::lock_guard<std::mutex> scoped_lock(mutex_);
        auto it = Find(key);
        if (it == list_.end()) {
            return std::nullopt;
        }

        return std::make_optional(it->Value());
    }

    std::optional<std::pair<K, V>> RemoveOldest() {
        std::lock_guard<std::mutex> scoped_lock;
        return RemoveOldestItem();
    }

    size_t Resize(size_t size) {
        std::lock_guard<std::mutex> scoped_lock;
        return _Resize(size);
    }

    CacheError Replace(const K &key, const V &value) {
        std::lock_guard<std::mutex> scoped_lock;
        auto it = Find(key);
        if (it == list_.end()) {
            return CacheError::kNotFound;
        }

        it.second = CacheItem(key, value, it.second.Expiration());
        return CacheError::kOK;
    }

    void ClearExpiredData() {
        std::lock_guard<std::mutex> scoped_lock;
        if (list_.empty()) {
            return;
        }

        auto now_seconds = NowSeconds();
        _ClearExpiredData(now_seconds);
    }

    CacheError UpdateValue(const K &key, const V &value) {
        std::lock_guard<std::mutex> scoped_lock;

        auto v = Update(key, value, -1);
        if (!v) {
            return CacheError::kNotFound;
        }

        return CacheError::kOK;
    }

    CacheError UpdateExpirationDate(const K &key, std::chrono::seconds expiration) {
        std::lock_guard<std::mutex> scoped_lock;

        auto new_expiration = NowSeconds() + expiration;
        auto v = Update(key, value, new_expiration);
        if (!v) {
            return CacheError::kNotFound;
        }

        return CacheError::kOK;
    }

    size_t Capacity() const noexcept {
        return capacity_;
    }

  private:
    using item_iterator = typename std::list<CacheItem<K, V>>::iterator;

    // typename std::list<CacheItem<K, V>>::iterator Find(const K &key) {
    item_iterator Find(const K &key) {
        for (auto it = list_.begin(); it != list_.end(); ++it) {
            if (key == it->Key()) {
                return it;
            }
        }

        return list_.end();
    }

    void Delete(const K &key) {
        auto it = Find(key);
        if (it == list_.end()) {
            return;
        }

        list_.erase(it);
    }

    const CacheItem<K, V> &LRUItem() {
        return list_.back();
    }

    std::chrono::seconds NowSeconds() const {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
    }

    std::optional<std::pair<K, V>> _RemoveOldest() {
        if (list_.empty()) {
            return std::nullopt;
        }

        auto item = LRUItem();
        Delete(item.Key());
        return std::make_optional({item.Key(), item.Value()});
    }

    size_t _Resize(size_t size) {
        size_t diff = 0;
        if (size < list_.size()) {
            diff = list_.size() - size;
        }

        for (size_t i = 0; i < diff; ++i) {
            (void)_RemoveOldest();
        }

        capacity_ = size;
        return diff;
    }

    void _ClearExpiredData(std::chrono::seconds now) {
        for (auto it = list_.begin(); it != list_.end();) {
            if (it.Expiration() < now) {
                it = list_.erase(it);
            } else {
                ++it;
            }
        }
    }

    std::optional<CacheItem<K, V>> _Update(const K &key, const V &value, std::chrono::seconds expiration) {
        for (auto it = list_.begin(); it != list_.end(); ++it) {
            if (it->Key() == key) {
                auto item = *it;
                list_.erase(it);

                if (expiration == -1) {
                    expiration = it->Expiration();
                }

                auto new_item = CacheItem(key, value, expiration);
                list_.push_front(new_item);
                return std::make_optional(new_item);
            }
        }

        return std::nullopt;
    }

    size_t capacity_;
    mutable std::mutex mutex_;
    std::list<CacheItem<K, V>> list_;
};

int main() {
    {
        Cache<std::string, int> c(3);
        c.Add("foo", 100, std::chrono::seconds(1000));
        c.Add("bar", 99, std::chrono::seconds(2000));

        auto v = c.Get("foo");
        assert(v.has_value());
        assert(v.value() == 100);
    }

    printf("## OK ##\n");
    return 0;
}