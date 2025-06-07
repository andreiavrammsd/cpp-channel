// Copyright (C) 2020-2025 Andrei Avram

#ifndef MSD_CHANNEL_HPP_
#define MSD_CHANNEL_HPP_

#include <atomic>
#include <condition_variable>
#include <cstdlib>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <type_traits>

#include "blocking_iterator.hpp"

namespace msd {

#if (__cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L))
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif

/**
 * @brief Exception thrown if trying to write on closed channel.
 */
class closed_channel : public std::runtime_error {
   public:
    /**
     * @brief Constructs the exception with an error message.
     *
     * @param msg A descriptive message explaining the cause of the error.
     */
    explicit closed_channel(const char* msg) : std::runtime_error{msg} {}
};

/**
 * @brief Thread-safe container for sharing data between threads.
 *
 * Implements a blocking input iterator.
 *
 * @tparam T The type of the elements.
 */
template <typename T>
class channel {
   public:
    /**
     * @brief The type of elements stored in the channel.
     */
    using value_type = T;

    /**
     * @brief The iterator type used to traverse the channel.
     */
    using iterator = blocking_iterator<channel<T>>;

    /**
     * @brief The type used to represent sizes and counts.
     */
    using size_type = std::size_t;

    /**
     * @brief Creates an unbuffered channel.
     */
    constexpr channel() = default;

    /**
     * @brief Creates a buffered channel.
     *
     * @param capacity Number of elements the channel can store before blocking.
     */
    explicit constexpr channel(const size_type capacity) : cap_{capacity} {}

    /**
     * @brief Pushes an element into the channel.
     *
     * @throws closed_channel if channel is closed.
     */
    template <typename Type>
    friend channel<typename std::decay<Type>::type>& operator<<(channel<typename std::decay<Type>::type>&, Type&&);

    /**
     * @brief Pops an element from the channel.
     *
     * @tparam Type The type of the elements
     */
    template <typename Type>
    friend channel<Type>& operator>>(channel<Type>&, Type&);

    /**
     * @brief Returns the current size of the channel.
     *
     * @return The number of elements in the channel.
     */
    NODISCARD size_type constexpr size() const noexcept { return size_; }

    /**
     * @brief Checks if the channel is empty.
     *
     * @return true If the channel contains no elements.
     * @return false Otherwise.
     */
    NODISCARD bool constexpr empty() const noexcept { return size_ == 0; }

    /**
     * @brief Closes the channel.
     */
    void close() noexcept
    {
        {
            std::unique_lock<std::mutex> lock{mtx_};
            is_closed_.store(true);
        }
        cnd_.notify_all();
    }

    /**
     * @brief Checks if the channel has been closed.
     *
     * @return true If no more elements can be added to the channel.
     * @return false Otherwise.
     */
    NODISCARD bool closed() const noexcept { return is_closed_.load(); }

    /**
     * @brief Returns an iterator to the beginning of the channel.
     *
     * @return A blocking iterator pointing to the start of the channel.
     */
    iterator begin() noexcept { return blocking_iterator<channel<T>>{*this}; }

    /**
     * @brief Returns an iterator representing the end of the channel.
     *
     * @return A blocking iterator representing the end condition.
     */
    iterator end() noexcept { return blocking_iterator<channel<T>>{*this}; }

    /**
     * Channel cannot be copied or moved.
     */
    channel(const channel&) = delete;
    channel& operator=(const channel&) = delete;
    channel(channel&&) = delete;
    channel& operator=(channel&&) = delete;
    virtual ~channel() = default;

   private:
    const size_type cap_{0};
    std::queue<T> queue_;
    std::atomic<std::size_t> size_{0};
    std::mutex mtx_;
    std::condition_variable cnd_;
    std::atomic<bool> is_closed_{false};

    void waitBeforeRead(std::unique_lock<std::mutex>& lock)
    {
        cnd_.wait(lock, [this]() { return !empty() || closed(); });
    };

    void waitBeforeWrite(std::unique_lock<std::mutex>& lock)
    {
        if (cap_ > 0 && size_ == cap_) {
            cnd_.wait(lock, [this]() { return size_ < cap_; });
        }
    }

    friend class blocking_iterator<channel>;
};

template <typename T>
channel<typename std::decay<T>::type>& operator<<(channel<typename std::decay<T>::type>& ch, T&& in)
{
    {
        std::unique_lock<std::mutex> lock{ch.mtx_};
        ch.waitBeforeWrite(lock);

        if (ch.closed()) {
            throw closed_channel{"cannot write on closed channel"};
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
    {
        std::unique_lock<std::mutex> lock{ch.mtx_};
        ch.waitBeforeRead(lock);

        if (ch.closed() && ch.empty()) {
            return ch;
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

}  // namespace msd

#endif  // MSD_CHANNEL_HPP_
