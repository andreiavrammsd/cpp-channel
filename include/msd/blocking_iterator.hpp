// Copyright (C) 2021 Andrei Avram

#ifndef MSD_CHANNEL_BLOCKING_ITERATOR_HPP_
#define MSD_CHANNEL_BLOCKING_ITERATOR_HPP_

#include <iterator>

namespace msd {

/**
 *  @brief An iterator that block the current thread,
 *  waiting to fetch elements from the channel.
 *
 *  Used to implement channel range-based for loop.
 *
 *  @tparam Channel Instance of channel.
 */
template <typename channel>
class blocking_iterator {
   public:
    using value_type = typename channel::value_type;

    explicit blocking_iterator(channel& ch) : ch{ch} {}

    /**
     * Advances to next element in the channel.
     */
    blocking_iterator<channel> operator++() const noexcept { return *this; }

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
     * Makes iteration continue until the channel is closed and empty.
     */
    bool operator!=(blocking_iterator<channel>) const
    {
        ch.waitBeforeRead();

        return !(ch.closed() && ch.empty());
    }

   private:
    channel& ch;
};

}  // namespace msd

/**
 * @brief Output iterator specialization
 */
template <typename T>
struct std::iterator_traits<msd::blocking_iterator<T>> {
    using value_type = typename msd::blocking_iterator<T>::value_type;
    using iterator_category = std::output_iterator_tag;
};

#endif  // MSD_CHANNEL_BLOCKING_ITERATOR_HPP_
