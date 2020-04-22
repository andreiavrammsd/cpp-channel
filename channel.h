#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <cstdlib>
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class const_iterator;

template<typename T>
class Channel {
public:
    explicit Channel(size_t capacity = 0);

    Channel(const Channel &) = delete;

    template<typename Q>
    friend void operator>>(Q, Channel<Q> &);

    template<typename Q>
    friend Q operator<<(Q &, Channel<Q> &);

    size_t size() const;

    const_iterator<T> begin() noexcept;

    const_iterator<T> end() noexcept;

private:
    size_t cap;
    std::queue<T> queue;
    std::mutex mtx;
    std::condition_variable cnd;
};

template<typename T>
class const_iterator {
public:
    explicit const_iterator(Channel<T> *ch) : ch{ch} {}

    const_iterator<T> operator++() {
        return *this;
    }

    T operator*() {
        T i{};
        i << *ch;
        return std::move(i);
    }

    bool operator!=(const_iterator<T>) {
        return true;
    }

private:
    Channel<T> *ch;
};

#include "channel.cpp"

#endif // CHANNEL_H_
