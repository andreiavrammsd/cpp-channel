#ifndef CHANNEL_HPP_
#define CHANNEL_HPP_

#include <condition_variable>
#include <mutex>
#include <queue>

#if (__cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L))
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif

/**
 * Blocking infinite iterator
 *
 * Used to implement channel range-based for loop
 *
 * @tparam T
 */
template <typename T>
class channel_iterator;

/**
 * Channel for safe passing any data between threads
 *
 * @tparam T Type of object to store
 */
template <typename T>
class Channel {
   public:
    using value_type = T;
    using iterator = channel_iterator<T>;
    using size_type = size_t;

    /**
     * Channel constructor
     *
     * @param capacity Number of elements the channel can store before blocking
     */
    explicit constexpr Channel(size_type capacity = 0);

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
     */
    template <typename Q>
    friend void operator<<(Q&, Channel<Q>&);

    /**
     * size
     *
     * @return number of elements in channel
     */
    NODISCARD size_type constexpr size() const;

    /**
     * empty
     *
     * @return true if there are no elements in channel
     */
    NODISCARD bool constexpr empty() const;

    iterator begin() noexcept;

    iterator end() noexcept;

    /**
     * Channel cannot be copied or moved
     */
    Channel(const Channel&) = delete;

    Channel& operator=(const Channel&) = delete;

    Channel(Channel&&) = delete;

    Channel& operator=(Channel&&) = delete;

    ~Channel() = default;

   private:
    const size_type cap;
    std::queue<T> queue;
    std::mutex mtx;
    std::condition_variable cnd;
};

template <typename T>
class channel_iterator {
   public:
    explicit channel_iterator(Channel<T>& ch) : ch{ch} {}

    channel_iterator<T> operator++() { return *this; }

    T operator*()
    {
        T value{};
        value << ch;

        return value;
    }

    bool operator!=(channel_iterator<T>) { return true; }

   private:
    Channel<T>& ch;
};

#include "channel.cpp"

#endif  // CHANNEL_HPP_
