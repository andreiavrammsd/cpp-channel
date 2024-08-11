// Copyright (C) 2023 Andrei Avram

#ifndef MSD_CHANNEL_BLOCKING_ITERATOR_HPP_
#define MSD_CHANNEL_BLOCKING_ITERATOR_HPP_

#include <iterator>
#include <mutex>

namespace msd {

/**
 * @brief An iterator that block the current thread,
 * waiting to fetch elements from the channel.
 *
 * Used to implement channel range-based for loop.
 *
 * @tparam Channel Instance of channel.
 */
template <typename Channel>
class blocking_iterator {
   public:
    using value_type = typename Channel::value_type;
    using reference = const typename Channel::value_type&;

    explicit blocking_iterator(Channel& chan) : chan_{chan} {}

    /**
     * Advances to next element in the channel.
     */
    blocking_iterator<Channel> operator++() const noexcept { return *this; }

    /**
     * Returns an element from the channel.
     */
    reference operator*()
    {
        chan_ >> value_;

        return value_;
    }

    /**
     * Makes iteration continue until the channel is closed and empty.
     */
    bool operator!=(blocking_iterator<Channel>) const
    {
        std::unique_lock<std::mutex> lock{chan_.mtx_};
        chan_.waitBeforeRead(lock);

        return !(chan_.closed() && chan_.empty());
    }

   private:
    Channel& chan_;
    value_type value_{};
};

}  // namespace msd

/**
 * @brief Input iterator specialization
 */
template <typename T>
struct std::iterator_traits<msd::blocking_iterator<T>> {
    using value_type = typename msd::blocking_iterator<T>::value_type;
    using reference = typename msd::blocking_iterator<T>::reference;
    using iterator_category = std::input_iterator_tag;
};

#endif  // MSD_CHANNEL_BLOCKING_ITERATOR_HPP_
