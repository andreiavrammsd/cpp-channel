// Copyright (C) 2023 Andrei Avram
namespace msd {

template <typename T>
constexpr channel<T>::channel(const size_type capacity) : cap_{capacity}
{
}

template <typename T>
template <typename Rep, typename Period>
void channel<T>::setTimeout(const std::chrono::duration<Rep, Period>& timeout)
{
    timeout_ = std::chrono::duration_cast<std::chrono::nanoseconds>(timeout);
}

template <typename T>
void channel<T>::clearTimeout() noexcept
{
    timeout_ = std::chrono::nanoseconds::zero();
}

template <typename T>
template <typename Predicate>
bool channel<T>::waitWithTimeout(std::unique_lock<std::mutex>& lock, Predicate pred)
{
    auto timeout = timeout_.load(std::memory_order_relaxed);
    if (timeout == std::chrono::nanoseconds::zero()) {
        cnd_.wait(lock, pred);
        return true;
    }

    return cnd_.wait_for(lock, timeout, pred);
}

template <typename T>
bool channel<T>::waitBeforeRead(std::unique_lock<std::mutex>& lock)
{
    return waitWithTimeout(lock, [this]() { return !empty() || closed(); });
}

template <typename T>
bool channel<T>::waitBeforeWrite(std::unique_lock<std::mutex>& lock)
{
    if (cap_ > 0 && size_ == cap_) {
        return waitWithTimeout(lock, [this]() { return size_ < cap_; });
    }
    return true;
}

template <typename T>
channel<typename std::decay<T>::type>& operator<<(channel<typename std::decay<T>::type>& ch, T&& in)
{
    if (ch.closed()) {
        throw closed_channel{"cannot write on closed channel"};
    }
    {
        std::unique_lock<std::mutex> lock{ch.mtx_};
        if (!ch.waitBeforeWrite(lock)) {
            throw channel_timeout{"write operation timed out"};
        }
        ch.queue_.push(std::forward<T>(in));
        ++ch.size_;
    }
    ch.cnd_.notify_one();
    return ch;
}

template <typename T>
channel<T>& operator>>(channel<T>& ch, T& out)
{
    if (ch.closed() && ch.empty()) {
        return ch;
    }
    {
        std::unique_lock<std::mutex> lock{ch.mtx_};
        if (!ch.waitBeforeRead(lock)) {
            throw channel_timeout{"read operation timed out"};
        }
        if (!ch.empty()) {
            out = std::move(ch.queue_.front());
            ch.queue_.pop();
            --ch.size_;
        }
    }
    ch.cnd_.notify_one();
    return ch;
}

template <typename T>
constexpr typename channel<T>::size_type channel<T>::size() const noexcept
{
    return size_;
}

template <typename T>
constexpr bool channel<T>::empty() const noexcept
{
    return size_ == 0;
}

template <typename T>
void channel<T>::close() noexcept
{
    {
        std::unique_lock<std::mutex> lock{mtx_};
        is_closed_.store(true, std::memory_order_relaxed);
    }
    cnd_.notify_all();
}

template <typename T>
bool channel<T>::closed() const noexcept
{
    return is_closed_.load(std::memory_order_relaxed);
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

}  // namespace msd
