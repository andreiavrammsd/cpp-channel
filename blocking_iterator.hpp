#ifndef BLOCKING_ITERATOR_HPP_
#define BLOCKING_ITERATOR_HPP_

template <typename T>
class Channel;

/**
 *  @brief An iterator that block the current thread,
 *  waiting to fetch elements from the channel.
 *
 *  Used to implement channel range-based for loop.
 *
 *  @tparam T Type of element in channel.
 */
template <typename T>
class blocking_iterator {
   public:
    explicit blocking_iterator(Channel<T>& ch) : ch{ch} {}

    /**
     * Advances to next element in the channel.
     */
    blocking_iterator<T> operator++() { return *this; }

    /**
     * Returns an element from the channel.
     */
    T operator*()
    {
        T value{};
        value << ch;

        return value;
    }

    /**
     * Makes iteration infinite.
     */
    bool operator!=(blocking_iterator<T>) { return true; }

   private:
    Channel<T>& ch;
};

#endif  // BLOCKING_ITERATOR_HPP_
