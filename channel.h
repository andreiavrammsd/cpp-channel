#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <cstdlib>
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class Channel {
public:
    explicit Channel(size_t capacity = 0);

    Channel(const Channel &) = delete;

    template<typename Q>
    friend void operator>>(Q, Channel<Q> &);

    template<typename Q>
    friend Q operator<<(Q &, Channel<Q> &);

    size_t size();

private:
    size_t cap;
    std::queue<T> queue;
    std::mutex mtx;
    std::condition_variable cnd;
};

#include "channel.cpp"

#endif // CHANNEL_H_
