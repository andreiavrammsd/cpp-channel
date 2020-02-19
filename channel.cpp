#include <utility>

template<typename T>
Channel<T>::Channel(size_t capacity) : cap{capacity} {}

template<typename Q>
void operator>>(Q in, Channel<Q> &ch) {
    std::unique_lock<std::mutex> lock{ch.mtx};

    if (ch.cap > 0 && ch.queue.size() == ch.cap) {
        ch.cnd.wait(lock, [&ch]() { return ch.queue.size() < ch.cap; });
    }

    ch.queue.push(std::move(in));

    ch.cnd.notify_one();
}

template<typename Q>
Q operator<<(Q &out, Channel<Q> &ch) {
    std::unique_lock<std::mutex> lock{ch.mtx};

    ch.cnd.wait(lock, [&ch] { return ch.queue.size() > 0; });

    auto value = ch.queue.front();
    out = value;
    ch.queue.pop();

    ch.cnd.notify_one();

    return out;
}

template<typename T>
size_t Channel<T>::size() {
    return queue.size();
}
