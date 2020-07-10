#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <condition_variable>
#include <mutex>
#include <queue>

/**
 * Blocking infinite iterator
 *
 * Used to implement channel range-based for loop
 *
 * @tparam T
 */
template <typename T>
class const_iterator;

/**
 * Channel for safe passing any data between threads
 *
 * @tparam T
 */
template <typename T>
class Channel {
   public:
    /**
     * Channel constructor
     *
     * @param capacity Number of elements the channel can store before blocking
     */
    explicit constexpr Channel(size_t capacity = 0);

    /**
     * Channel cannot be copied or moved
     */
    Channel(const Channel&) = delete;

    Channel& operator=(const Channel&) = delete;

    Channel(Channel&&) = delete;

    Channel& operator=(Channel&&) = delete;

    /**
     * >> Push item of type Q to channel
     *
     * @tparam Q
     */
    template <typename Q>
    friend void operator>>(Q, Channel<Q>&);

    /**
     * << Fetch item from channel
     *
     * @tparam Q
     * @return Item of type Q
     */
    template <typename Q>
    friend Q operator<<(Q&, Channel<Q>&);

    /**
     * size
     *
     * @return number of elements in channel
     */
    size_t constexpr size() const;

    const_iterator<T> begin() noexcept;

    const_iterator<T> end() noexcept;

    ~Channel() = default;

   private:
    const size_t cap;
    std::queue<T> queue;
    std::mutex mtx;
    std::condition_variable cnd;

    inline T get();

    friend class const_iterator<T>;
};

template <typename T>
class const_iterator {
   public:
    explicit const_iterator(Channel<T>* ch) : ch{ch} {}

    const_iterator<T> operator++() { return *this; }

    T operator*() { return ch->get(); }

    bool operator!=(const_iterator<T>) { return true; }

   private:
    Channel<T>* ch;
};

#include "channel.cpp"

#endif  // CHANNEL_H_
