// Copyright (C) 2021 Andrei Avram

#include <utility>

template <typename T>
constexpr channel<T>::channel(const size_type capacity) : cap{capacity}, is_closed{false}
{
}

template <typename T>
void operator>>(const T& in, channel<T>& ch)
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
void operator>>(T&& in, channel<T>& ch)
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
void operator<<(T& out, channel<T>& ch)
{
    if (ch.closed() && ch.empty()) {
        return;
    }

    std::unique_lock<std::mutex> lock{ch.mtx};
    ch.cnd.wait(lock, [&ch] { return ch.queue.size() > 0 || ch.closed(); });

    if (ch.queue.size() > 0) {
        out = std::move(ch.queue.front());
        ch.queue.pop();
    }
    ch.cnd.notify_one();
}

template <typename T>
constexpr typename channel<T>::size_type channel<T>::size() const noexcept
{
    return queue.size();
}

template <typename T>
constexpr bool channel<T>::empty() const noexcept
{
    return queue.empty();
}

template <typename T>
void channel<T>::close() noexcept
{
    cnd.notify_one();
    is_closed.store(true);
}

template <typename T>
bool channel<T>::closed() const noexcept
{
    return is_closed.load();
}

template <typename T>
BlockingIterator<channel<T>> channel<T>::begin() noexcept
{
    return BlockingIterator<channel<T>>{*this};
}

template <typename T>
BlockingIterator<channel<T>> channel<T>::end() noexcept
{
    return BlockingIterator<channel<T>>{*this};
}
