// Copyright (C) 2020-2025 Andrei Avram

#ifndef MSD_STATIC_CHANNEL_HPP_
#define MSD_STATIC_CHANNEL_HPP_

#include <array>
#include <condition_variable>
#include <cstdlib>
#include <mutex>

#include "blocking_iterator.hpp"
#include "channel.hpp"

namespace msd {

/**
 * @brief Thread-safe container for sharing data between threads.
 *
 * Allocates on the stack.
 * Does not throw exceptions.
 * Implements a blocking input iterator.
 *
 * @tparam T The type of the elements.
 * @tparam Capacity The maximum number of elements the channel can hold before blocking.
 */
template <typename T, std::size_t Capacity>
class static_channel {
   public:
    static_assert(Capacity > 0, "Channel capacity must be greater than zero.");

    /**
     * @brief The type of elements stored in the channel.
     */
    using value_type = T;

    /**
     * @brief The iterator type used to traverse the channel.
     */
    using iterator = blocking_iterator<static_channel<T, Capacity>>;

    /**
     * @brief The type used to represent sizes and counts.
     */
    using size_type = std::size_t;

    /**
     * @brief Creates a new channel.
     */
    constexpr static_channel() = default;

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

            array_[(front_ + size_) % Capacity] = std::forward<Type>(value);
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

            out = std::move(array_[front_]);
            front_ = (front_ + 1) % Capacity;
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
        return is_closed_ && size_ == 0;
    }

    /**
     * @brief Returns an iterator to the beginning of the channel.
     *
     * @return A blocking iterator pointing to the start of the channel.
     */
    iterator begin() noexcept { return blocking_iterator<static_channel<T, Capacity>>{*this}; }

    /**
     * @brief Returns an iterator representing the end of the channel.
     *
     * @return A blocking iterator representing the end condition.
     */
    iterator end() noexcept { return blocking_iterator<static_channel<T, Capacity>>{*this}; }

    /**
     * Channel cannot be copied or moved.
     */
    static_channel(const static_channel&) = delete;
    static_channel& operator=(const static_channel&) = delete;
    static_channel(static_channel&&) = delete;
    static_channel& operator=(static_channel&&) = delete;
    virtual ~static_channel() = default;

   private:
    std::array<T, Capacity> array_{};
    size_type front_{0};
    std::size_t size_{0};
    const size_type cap_{Capacity};
    mutable std::mutex mtx_;
    std::condition_variable cnd_;
    bool is_closed_{false};

    void waitBeforeRead(std::unique_lock<std::mutex>& lock)
    {
        cnd_.wait(lock, [this]() { return !(size_ == 0) || is_closed_; });
    };

    void waitBeforeWrite(std::unique_lock<std::mutex>& lock)
    {
        if (cap_ > 0 && size_ == cap_) {
            cnd_.wait(lock, [this]() { return size_ < cap_; });
        }
    }
};

}  // namespace msd

#endif  // MSD_STATIC_CHANNEL_HPP_
