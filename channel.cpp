#include <utility>

template <typename T>
constexpr Channel<T>::Channel(const size_type capacity) : cap{capacity}
{
}

template <typename Q>
void operator>>(const Q in, Channel<Q>& ch)
{
    std::unique_lock<std::mutex> lock{ch.mtx};

    if (ch.cap > 0 && ch.queue.size() == ch.cap) {
        ch.cnd.wait(lock, [&ch]() { return ch.queue.size() < ch.cap; });
    }

    ch.queue.push(std::move(in));

    ch.cnd.notify_one();
}

template <typename Q>
void operator<<(Q& out, Channel<Q>& ch)
{
    std::unique_lock<std::mutex> lock{ch.mtx};
    ch.cnd.wait(lock, [&ch] { return ch.queue.size() > 0; });

    out = ch.queue.front();
    ch.queue.pop();

    ch.cnd.notify_one();
}

template <typename T>
constexpr typename Channel<T>::size_type Channel<T>::size() const
{
    return queue.size();
}

template <typename T>
const_iterator<T> Channel<T>::begin() noexcept
{
    return const_iterator<T>{*this};
}

template <typename T>
const_iterator<T> Channel<T>::end() noexcept
{
    return const_iterator<T>{*this};
}
