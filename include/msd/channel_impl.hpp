// Copyright (C) 2023 Andrei Avram

template <typename T>
constexpr channel<T>::channel(const size_type capacity) : cap_{capacity}
{
}

template <typename T>
void operator>>(T&& in, channel<typename std::decay<T>::type>& ch)
{
    if (ch.closed()) {
        throw closed_channel{"cannot write on closed channel"};
    }

    {
        std::unique_lock<std::mutex> lock{ch.mtx_};
        ch.waitBeforeWrite(lock);

        ch.queue_.push(std::forward<T>(in));
        ++ch.size_;
    }

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

        if (!ch.empty()) {
            out = std::move(ch.queue_.front());
            ch.queue_.pop();
            --ch.size_;
        }
    }

    ch.cnd_.notify_one();
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
    cnd_.wait(lock, [this]() { return !empty() || closed(); });
}

template <typename T>
void channel<T>::waitBeforeWrite(std::unique_lock<std::mutex>& lock)
{
    if (cap_ > 0 && size_ == cap_) {
        cnd_.wait(lock, [this]() { return size_ < cap_; });
    }
}
