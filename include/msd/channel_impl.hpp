// Copyright (C) 2022 Andrei Avram

#include <utility>

template <typename T>
constexpr channel<T>::channel(const size_type capacity) : cap_{capacity}
{
}

template <typename T>
void operator>>(const T& in, channel<T>& ch)
{
    if (ch.closed()) {
        throw closed_channel{"cannot write on closed channel"};
    }

    std::unique_lock<std::mutex> lock{ch.mtx_};

    if (ch.cap_ > 0 && ch.queue_.size() == ch.cap_) {
        ch.cnd_.wait(lock, [&ch]() { return ch.queue_.size() < ch.cap_; });
    }

    ch.queue_.push(in);

    ch.cnd_.notify_one();
}

template <typename T>
void operator>>(T&& in, channel<T>& ch)
{
    if (ch.closed()) {
        throw closed_channel{"cannot write on closed channel"};
    }

    std::unique_lock<std::mutex> lock{ch.mtx_};

    if (ch.cap_ > 0 && ch.queue_.size() == ch.cap_) {
        ch.cnd_.wait(lock, [&ch]() { return ch.queue_.size() < ch.cap_; });
    }

    ch.queue_.push(std::forward<T>(in));

    ch.cnd_.notify_one();
}

template <typename T>
void operator<<(T& out, channel<T>& ch)
{
    if (ch.closed() && ch.empty()) {
        return;
    }

    {
        std::unique_lock<std::mutex> lock{ch.mtx_};
        ch.waitBeforeRead(lock);

        if (ch.queue_.size() > 0) {
            out = std::move(ch.queue_.front());
            ch.queue_.pop();
        }
    }

    ch.cnd_.notify_one();
}

template <typename T>
constexpr typename channel<T>::size_type channel<T>::size() const noexcept
{
    return queue_.size();
}

template <typename T>
constexpr bool channel<T>::empty() const noexcept
{
    return queue_.empty();
}

template <typename T>
void channel<T>::close() noexcept
{
    is_closed_.store(true);
    cnd_.notify_all();
}

template <typename T>
bool channel<T>::closed() const noexcept
{
    return is_closed_.load();
}

template <typename T>
blocking_iterator<channel<T>> channel<T>::begin() noexcept
{
    return blocking_iterator<channel<T>>{*this};
}

template <typename T>
blocking_iterator<channel<T>> channel<T>::end() noexcept
{
    return blocking_iterator<channel<T>>{*this};
}

template <typename T>
void channel<T>::waitBeforeRead(std::unique_lock<std::mutex>& lock)
{
    cnd_.wait(lock, [this] { return queue_.size() > 0 || closed(); });
}
