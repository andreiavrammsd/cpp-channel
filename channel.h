#ifndef CPP_CHANNEL_CHANNEL_H
#define CPP_CHANNEL_CHANNEL_H

#include <cstdlib>
#include <queue>

template<typename T>
class Channel {
public:
    explicit Channel(size_t cap = 0) : cap(cap) {}

    template<typename Q>
    friend void operator>>(Q, Channel<Q> &);

    template<typename Q>
    friend Q operator<<(Q &, Channel<Q> &);

    size_t size();

private:
    size_t cap;
    std::queue<T> queue;
};

template<typename Q>
void operator>>(Q in, Channel<Q> &ch) {
    ch.queue.push(in);
}

template<typename Q>
Q operator<<(Q &out, Channel<Q> &ch) {
    auto value = ch.queue.front();
    ch.queue.pop();

    out = value;

    return out;
}

template<typename T>
size_t Channel<T>::size() {
    return queue.size();
}

#endif //CPP_CHANNEL_CHANNEL_H
