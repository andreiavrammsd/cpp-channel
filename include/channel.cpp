#include <utility>

template <typename T>
constexpr Channel<T>::Channel(const size_type capacity) : cap{capacity}, is_closed{false}
{
}

template <typename T>
void operator>>(const T& in, Channel<T>& ch)
{
    if (ch.closed()) {
        throw ClosedChannel{"cannot write on closed channel"};
    }

    std::unique_lock<std::mutex> lock{ch.mtx};

    if (ch.cap > 0 && ch.queue.size() == ch.cap) {
        ch.cnd.wait(lock, [&ch]() { return ch.queue.size() < ch.cap; });
    }

    ch.queue.push(in);

    ch.cnd.notify_one();
}

template <typename T>
void operator>>(T&& in, Channel<T>& ch)
{
    if (ch.closed()) {
        throw ClosedChannel{"cannot write on closed channel"};
    }

    std::unique_lock<std::mutex> lock{ch.mtx};

    if (ch.cap > 0 && ch.queue.size() == ch.cap) {
        ch.cnd.wait(lock, [&ch]() { return ch.queue.size() < ch.cap; });
    }

    ch.queue.push(std::forward<T>(in));

    ch.cnd.notify_one();
}

template <typename T>
void operator<<(T& out, Channel<T>& ch)
{
    if (ch.closed() && ch.empty()) {
        return;
    }

    std::unique_lock<std::mutex> lock{ch.mtx};
    ch.cnd.wait(lock, [&ch] { return ch.queue.size() > 0 || ch.closed(); });

    if (!ch.empty()) {
        out = std::move(ch.queue.front());
        ch.queue.pop();
    }
    ch.cnd.notify_one();
}

template <typename T>
constexpr typename Channel<T>::size_type Channel<T>::size() const noexcept
{
    return queue.size();
}

template <typename T>
constexpr bool Channel<T>::empty() const noexcept
{
    return queue.empty();
}

template <typename T>
void Channel<T>::close() noexcept
{
    cnd.notify_one();
    is_closed.store(true);
}

template <typename T>
bool Channel<T>::closed() const noexcept
{
    return is_closed.load();
}

template <typename T>
BlockingIterator<Channel<T>> Channel<T>::begin() noexcept
{
    return BlockingIterator<Channel<T>>{*this};
}

template <typename T>
BlockingIterator<Channel<T>> Channel<T>::end() noexcept
{
    return BlockingIterator<Channel<T>>{*this};
}
