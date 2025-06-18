// Copyright (C) 2020-2025 Andrei Avram

#ifndef MSD_CHANNEL_CHANNEL_HPP_
#define MSD_CHANNEL_CHANNEL_HPP_

#include "blocking_iterator.hpp"
#include "nodiscard.hpp"
#include "result.hpp"
#include "storage.hpp"

#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <mutex>
#include <stdexcept>
#include <type_traits>

/** @file */

namespace msd {

/**
 * @brief Default storage for msd::channel.
 *
 * @tparam T The type of the elements.
 * @typedef default_storage
 */
template <typename T>
using default_storage = queue_storage<T>;

/**
 * @brief Trait to check if a type is supported by msd::channel.
 *
 * This trait ensures the type meets all requirements to be safely used
 * within the channel:
 * - Default constructible: must be able to create a default instance.
 * - Move constructible: must be movable to allow efficient element transfer.
 * - Move assignable: must support move assignment for storage management.
 * - Destructible: must have a valid destructor.
 *
 * @tparam T The type to check.
 */
template <typename T>
struct is_supported_type {
    /**
     * @brief Indicates if the type meets all channel requirements.
     */
    static constexpr bool value = std::is_default_constructible<T>::value && std::is_move_constructible<T>::value &&
                                  std::is_move_assignable<T>::value && std::is_destructible<T>::value;
};

/**
 * @brief Trait to check if a storage type has a static **capacity** member.
 */
template <typename, typename = void>
struct is_static_storage : std::false_type {};

/**
 * @brief Trait to check if a storage type has a static **capacity** member.
 *
 * @tparam Storage The storage type to check.
 */
template <typename Storage>
struct is_static_storage<Storage, decltype((void)Storage::capacity, void())> : std::true_type {};

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
 * @brief Possible errors during a batch write operation.
 */
enum class batch_write_error : std::int8_t {
    /**
     * @brief The specified range exceeds the available capacity.
     */
    range_exceeds_capacity,

    /**
     * @brief The receiving channel is closed and cannot accept data.
     */
    channel_is_closed,
};

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
    static_assert(is_supported_type<T>::value, "Type T does not meet all requirements.");

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
     * @param chan Channel to write to.
     * @param value Value to write.
     * @return Instance of channel.
     * @throws closed_channel if channel is closed.
     */
    template <typename Type, typename Store>
    friend channel<typename std::decay<Type>::type, Store>& operator<<(
        channel<typename std::decay<Type>::type, Store>& chan, Type&& value);

    /**
     * @brief Pops an element from the channel.
     *
     * @param chan Channel to read from.
     * @param out Where to write read value.
     * @return Instance of channel.
     */
    template <typename Type, typename Store>
    friend channel<Type, Store>& operator>>(channel<Type, Store>& chan, Type& out);

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
     * @brief Writes a range of elements into the channel in batch mode.
     *
     * This function attempts to write all elements from the input range [begin, end)
     * into the channel. If the channel has a capacity and the range exceeds that capacity,
     * the function returns an error. If the channel is closed, it also returns an error.
     *
     * @tparam InputIterator An input iterator type pointing to elements of type `T`.
     * @param begin Iterator pointing to the beginning of the range to write.
     * @param end Iterator pointing to the end of the range to write (exclusive).
     * @return A result indicating success or containing a `batch_write_error`:
     *   - `batch_write_error::range_exceeds_capacity` if the range is too large to fit.
     *   - `batch_write_error::channel_is_closed` if the channel is already closed.
     *   - Empty (success) if all elements were successfully written.
     *
     * @note It takes the lock on the channel once for all elements and release it at the end.
     */
    template <typename InputIterator>
    result<void, batch_write_error> batch_write(const InputIterator begin, const InputIterator end)
    {
        {
            std::unique_lock<std::mutex> lock{mtx_};
            wait_before_write(lock);

            if (is_closed_) {
                return result<void, batch_write_error>{batch_write_error::channel_is_closed};
            }

            if (capacity_ > 0 && (static_cast<std::size_t>(std::distance(begin, end)) + storage_.size()) > capacity_) {
                return result<void, batch_write_error>{batch_write_error::range_exceeds_capacity};
            }

            for (InputIterator it = begin; it != end; ++it) {
                storage_.push_back(*it);
            }
        }

        cnd_.notify_one();

        return result<void, batch_write_error>{};
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
 * @copydoc msd::channel::operator<<
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
 * @copydoc msd::channel::operator>>
 */
template <typename T, typename Storage>
channel<T, Storage>& operator>>(channel<T, Storage>& chan, T& out)
{
    chan.read(out);

    return chan;
}

}  // namespace msd

#endif  // MSD_CHANNEL_CHANNEL_HPP_
