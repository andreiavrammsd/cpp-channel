#ifndef BLOCKING_ITERATOR_HPP_
#define BLOCKING_ITERATOR_HPP_

/**
 *  @brief An iterator that block the current thread,
 *  waiting to fetch elements from the channel.
 *
 *  Used to implement channel range-based for loop.
 *
 *  @tparam Channel Instance of channel.
 */
template <typename Channel>
class BlockingIterator {
   public:
    using value_type = typename Channel::value_type;

    explicit BlockingIterator(Channel& ch) : ch{ch} {}

    /**
     * Advances to next element in the channel.
     */
    BlockingIterator<Channel> operator++() const noexcept { return *this; }

    /**
     * Returns an element from the channel.
     */
    value_type operator*() const
    {
        value_type value{};
        value << ch;

        return value;
    }

    /**
     * Makes iteration infinite.
     */
    bool operator!=(BlockingIterator<Channel>) const noexcept { return true; }

   private:
    Channel& ch;
};

#endif  // BLOCKING_ITERATOR_HPP_
