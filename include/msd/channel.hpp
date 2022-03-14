// Copyright (C) 2022 Andrei Avram

#ifndef MSD_CHANNEL_HPP_
#define MSD_CHANNEL_HPP_

#include <atomic>
#include <condition_variable>
#include <cstdlib>
#include <mutex>
#include <queue>
#include <stdexcept>

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
    explicit closed_channel(const char* msg) : std::runtime_error(msg) {}
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
    using value_type = T;
    using iterator = blocking_iterator<channel<T>>;
    using size_type = std::size_t;

    /**
     * Creates a new channel.
     *
     * @param capacity Number of elements the channel can store before blocking.
     */
    explicit constexpr channel(size_type capacity = 0);

    /**
     * Pushes an element into the channel by copy.
     *
     * @tparam Type The type of the elements.
     *
     * @throws closed_channel if channel is closed.
     */
    template <typename Type>
    friend void operator>>(const Type&, channel<Type>&);

    /**
     * Allows pushing an element into the channel by move.
     *
     * @tparam Type The type of the elements.
     *
     * @throws closed_channel if channel is closed.
     */
    template <typename Type>
    friend void operator>>(Type&&, channel<Type>&);

    /**
     * Pops an element from the channel.
     *
     * @tparam Type The type of the elements
     */
    template <typename Type>
    friend void operator<<(Type&, channel<Type>&);

    /**
     * Returns the number of elements in the channel.
     */
    NODISCARD inline size_type constexpr size() const noexcept;

    /**
     * Returns true if there are no elements in channel.
     */
    NODISCARD inline bool constexpr empty() const noexcept;

    /**
     * Closes the channel.
     */
    inline void close() noexcept;

    /**
     * Returns true if the channel is closed.
     */
    NODISCARD inline bool closed() const noexcept;

    /**
     * Iterator
     */
    iterator begin() noexcept;
    iterator end() noexcept;

    /**
     * Channel cannot be copied or moved.
     */
    channel(const channel&) = delete;
    channel& operator=(const channel&) = delete;
    channel(channel&&) = delete;
    channel& operator=(channel&&) = delete;
    virtual ~channel() = default;

   private:
    const size_type cap_;
    std::queue<T> queue_;
    std::mutex mtx_;
    std::condition_variable cnd_;
    std::atomic<bool> is_closed_{false};

    inline void waitBeforeRead();
    friend class blocking_iterator<channel>;
};

#include "channel_impl.hpp"

}  // namespace msd

#endif  // MSD_CHANNEL_HPP_
