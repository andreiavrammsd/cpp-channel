// Copyright (C) 2020-2025 Andrei Avram

#ifndef MSD_CHANNEL_CHANNEL_HPP_
#define MSD_CHANNEL_CHANNEL_HPP_

#include <condition_variable>
#include <cstdlib>
#include <mutex>
#include <stdexcept>
#include <type_traits>

#include "blocking_iterator.hpp"
#include "nodiscard.hpp"
#include "storage.hpp"

/** @file */

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
 * @brief Default storage for msd::channel.
 *
 * @tparam T The type of the elements.
 * @typedef default_storage
 */
template <typename T>
using default_storage = queue_storage<T>;

/**
 * @brief Trait to check if a type has a static **capacity** member.
 */
template <typename, typename = void>
struct is_static_storage : std::false_type {};

/**
 * @brief Trait to check if a type has a static **capacity** member.
 *
 * @tparam Storage The storage type to check.
 */
template <typename Storage>
struct is_static_storage<Storage, decltype((void)Storage::capacity, void())> : std::true_type {};

/**
 * @brief Thread-safe container for sharing data between threads.
 *
 * - Not movable, not copyable.
 * - Includes a blocking input iterator.
 *
 * @tparam T The type of the elements.
 * @tparam Storage The storage type used to hold the elements. Default: msd::queue_storage.
 */
template <typename T, typename Storage = default_storage<T>>
class channel {
   public:
    /**
     * @brief The type of elements stored in the channel.
     */
    using value_type = T;

    /**
     * @brief The iterator type used to traverse the channel.
     */
    using iterator = blocking_iterator<channel<T, Storage>>;

    /**
     * @brief The type used to represent sizes and counts.
     */
    using size_type = std::size_t;

    /**
     * @brief Creates a buffered channel if **Storage** is static (has static **capacity** member)
     *
     * @note Uses **Storage::capacity** as number of elements the channel can store before blocking.
     */
    template <typename S = Storage, typename std::enable_if<is_static_storage<S>::value, int>::type = 0>
    constexpr channel() : capacity_{Storage::capacity}
    {
    }

    /**
     * @brief Creates an unbuffered channel if **Storage** is not static (does not have static **capacity** member).
     */
    template <typename S = Storage, typename std::enable_if<!is_static_storage<S>::value, int>::type = 0>
    constexpr channel() : storage_{0}
    {
    }

    /**
     * @brief Creates a buffered channel if **Storage** is not static (does not have static **capacity** member).
     *
     * @param capacity Number of elements the channel can store before blocking.
     */
    template <typename S = Storage, typename std::enable_if<!is_static_storage<S>::value, int>::type = 0>
    explicit constexpr channel(const size_type capacity) : storage_{capacity}, capacity_{capacity}
    {
    }

    /**
     * @brief Pushes an element into the channel.
     *
     * @throws closed_channel if channel is closed.
     */
    template <typename Type, typename Store>
    friend channel<typename std::decay<Type>::type, Store>& operator<<(channel<typename std::decay<Type>::type, Store>&,
                                                                       Type&&);
    /**
     * @brief Pops an element from the channel.
     *
     * @tparam Type The type of the elements.
     */
    template <typename Type, typename Store>
    friend channel<Type, Store>& operator>>(channel<Type, Store>&, Type&);

    /**
     * @brief Pushes an element into the channel.
     *
     * @tparam Type The type of the elements.
     * @param value The element to be pushed into the channel.
     * @return true If an element was successfully pushed into the channel.
     * @return false If the channel is closed.
     */
    template <typename Type>
    bool write(Type&& value)
    {
        {
            std::unique_lock<std::mutex> lock{mtx_};
            wait_before_write(lock);

            if (is_closed_) {
                return false;
            }

            storage_.push_back(std::forward<Type>(value));
        }

        cnd_.notify_one();

        return true;
    }

    /**
     * @brief Pops an element from the channel.
     *
     * @param out Reference to the variable where the popped element will be stored.
     * @return true If an element was successfully read from the channel.
     * @return false If the channel is closed and empty.
     */
    bool read(T& out)
    {
        {
            std::unique_lock<std::mutex> lock{mtx_};
            wait_before_read(lock);

            if (storage_.size() == 0 && is_closed_) {
                return false;
            }

            storage_.pop_front(out);
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
        return storage_.size();
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
        return storage_.size() == 0;
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
        return storage_.size() == 0 && is_closed_;
    }

    /**
     * @brief Returns an iterator to the beginning of the channel.
     *
     * @return A blocking iterator pointing to the start of the channel.
     */
    iterator begin() noexcept { return blocking_iterator<channel<T, Storage>>{*this}; }

    /**
     * @brief Returns an iterator representing the end of the channel.
     *
     * @return A blocking iterator representing the end condition.
     */
    iterator end() noexcept { return blocking_iterator<channel<T, Storage>>{*this, true}; }

    channel(const channel&) = delete;
    channel& operator=(const channel&) = delete;
    channel(channel&&) = delete;
    channel& operator=(channel&&) = delete;
    virtual ~channel() = default;

   private:
    Storage storage_;
    std::condition_variable cnd_;
    mutable std::mutex mtx_;
    std::size_t capacity_{};
    bool is_closed_{};

    void wait_before_read(std::unique_lock<std::mutex>& lock)
    {
        cnd_.wait(lock, [this]() { return storage_.size() > 0 || is_closed_; });
    };

    void wait_before_write(std::unique_lock<std::mutex>& lock)
    {
        if (capacity_ > 0) {
            cnd_.wait(lock, [this]() { return storage_.size() < capacity_; });
        }
    }
};

/**
 * @copydoc msd::channel::write
 * @param chan
 */
template <typename T, typename Storage>
channel<typename std::decay<T>::type, Storage>& operator<<(channel<typename std::decay<T>::type, Storage>& chan,
                                                           T&& value)
{
    if (!chan.write(std::forward<T>(value))) {
        throw closed_channel{"cannot write on closed channel"};
    }

    return chan;
}

/**
 * @copydoc msd::channel::read
 * @param chan
 */
template <typename T, typename Storage>
channel<T, Storage>& operator>>(channel<T, Storage>& chan, T& out)
{
    chan.read(out);

    return chan;
}

}  // namespace msd

#endif  // MSD_CHANNEL_CHANNEL_HPP_
