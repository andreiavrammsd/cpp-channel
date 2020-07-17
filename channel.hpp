// Copyright (C) 2020 Andrei Avram

#ifndef CHANNEL_HPP_
#define CHANNEL_HPP_

#include <condition_variable>
#include <cstdlib>
#include <mutex>
#include <queue>

#include "blocking_iterator.hpp"

#if (__cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L))
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif

/**
 *  @brief A container for sharing elements between threads in a thread safe way.
 *
 *  Implements a blocking input iterator.
 *
 *  @tparam T Type of element.
 */
template <typename T>
class Channel {
   public:
    using value_type = T;
    using iterator = BlockingIterator<Channel<T>>;
    using size_type = std::size_t;

    /**
     * Creates a new channel.
     *
     * @param capacity Number of elements the channel can store before blocking.
     */
    explicit constexpr Channel(size_type capacity = 0);

    /**
     * Pushes an element into the channel by copy.
     */
    template <typename Q>
    friend void operator>>(const Q&, Channel<Q>&);

    /**
     * Allows pushing an element into the channel by move.
     */
    template <typename Q>
    friend void operator>>(Q&&, Channel<Q>&);

    /**
     * Returns an element from the channel.
     */
    template <typename Q>
    friend void operator<<(Q&, Channel<Q>&);

    /**
     * Returns the number of elements in the channel.
     */
    NODISCARD size_type constexpr size() const;

    /**
     *  Returns true if there are no elements in channel.
     */
    NODISCARD bool constexpr empty() const;

    /**
     * Iterator
     */
    iterator begin() noexcept;
    iterator end() noexcept;

    /**
     * Channel cannot be copied or moved.
     */
    Channel(const Channel&) = delete;
    Channel& operator=(const Channel&) = delete;
    Channel(Channel&&) = delete;
    Channel& operator=(Channel&&) = delete;
    virtual ~Channel() = default;

   private:
    const size_type cap;
    std::queue<T> queue;
    std::mutex mtx;
    std::condition_variable cnd;
};

#include "channel.cpp"

#endif  // CHANNEL_HPP_
