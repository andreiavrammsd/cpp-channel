// Copyright (C) 2020-2025 Andrei Avram

#ifndef MSD_CHANNEL_HPP_
#define MSD_CHANNEL_HPP_

#include <condition_variable>
#include <cstdlib>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <type_traits>

#include "blocking_iterator.hpp"
#include "nodiscard.hpp"

namespace msd {

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
     * @tparam Type The type of the elements.
     */
    template <typename Type>
    friend channel<Type>& operator>>(channel<Type>&, Type&);

    /**
     * @brief Pushes an element into the channel.
     *
     * @tparam Type The type of the elements.
     *
     * @param value The element to be pushed into the channel.
     *
     * @return true If an element was successfully pushed into the channel.
     * @return false If the channel is closed.
     */
    template <typename Type>
    bool write(Type&& value)
    {
        {
            std::unique_lock<std::mutex> lock{mtx_};
            waitBeforeWrite(lock);

            if (is_closed_) {
                return false;
            }

            queue_.push(std::forward<Type>(value));
            ++size_;
        }

        cnd_.notify_one();

        return true;
    }

    /**
     * @brief Pops an element from the channel.
     *
     * @param out Reference to the variable where the popped element will be stored.
     *
     * @return true If an element was successfully read from the channel.
     * @return false If the channel is closed and empty.
     */
    bool read(T& out)
    {
        {
            std::unique_lock<std::mutex> lock{mtx_};
            waitBeforeRead(lock);

            if (is_closed_ && size_ == 0) {
                return false;
            }

            out = std::move(queue_.front());
            queue_.pop();
            --size_;
        }

        cnd_.notify_one();

        return true;
    }

    /**
     * @brief Returns the current size of the channel.
     *
     * @return The number of elements in the channel.
     */
    NODISCARD size_type size() const noexcept
    {
        std::unique_lock<std::mutex> lock{mtx_};
        return size_;
    }

    /**
     * @brief Checks if the channel is empty.
     *
     * @return true If the channel contains no elements.
     * @return false Otherwise.
     */
    NODISCARD bool empty() const noexcept
    {
        std::unique_lock<std::mutex> lock{mtx_};
        return size_ == 0;
    }

    /**
     * @brief Closes the channel, no longer accepting new elements.
     */
    void close() noexcept
    {
        {
            std::unique_lock<std::mutex> lock{mtx_};
            is_closed_ = true;
        }
        cnd_.notify_all();
    }

    /**
     * @brief Checks if the channel has been closed.
     *
     * @return true If no more elements can be added to the channel.
     * @return false Otherwise.
     */
    NODISCARD bool closed() const noexcept
    {
        std::unique_lock<std::mutex> lock{mtx_};
        return is_closed_;
    }

    /**
     * @brief Checks if the channel has been closed and is empty.
     *
     * @return true If nothing can be read anymore from the channel.
     * @return false Otherwise.
     */
    NODISCARD bool drained() noexcept
    {
        std::unique_lock<std::mutex> lock{mtx_};
        return size_ == 0 && is_closed_;
    }

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
    iterator end() noexcept { return blocking_iterator<channel<T>>{*this, true}; }

    /**
     * Channel cannot be copied or moved.
     */
    channel(const channel&) = delete;
    channel& operator=(const channel&) = delete;
    channel(channel&&) = delete;
    channel& operator=(channel&&) = delete;
    virtual ~channel() = default;

   private:
    std::queue<T> queue_;
    std::size_t size_{0};
    const size_type cap_{0};
    mutable std::mutex mtx_;
    std::condition_variable cnd_;
    bool is_closed_{false};

    void waitBeforeRead(std::unique_lock<std::mutex>& lock)
    {
        cnd_.wait(lock, [this]() { return size_ > 0 || is_closed_; });
    };

    void waitBeforeWrite(std::unique_lock<std::mutex>& lock)
    {
        if (cap_ > 0 && size_ == cap_) {
            cnd_.wait(lock, [this]() { return size_ < cap_; });
        }
    }
};

template <typename T>
channel<typename std::decay<T>::type>& operator<<(channel<typename std::decay<T>::type>& chan, T&& value)
{
    if (!chan.write(std::forward<T>(value))) {
        throw closed_channel{"cannot write on closed channel"};
    }

    return chan;
}

template <typename T>
channel<T>& operator>>(channel<T>& chan, T& out)
{
    chan.read(out);

    return chan;
}

}  // namespace msd

#endif  // MSD_CHANNEL_HPP_
