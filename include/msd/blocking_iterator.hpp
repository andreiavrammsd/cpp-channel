// Copyright (C) 2020-2025 Andrei Avram

#ifndef MSD_CHANNEL_BLOCKING_ITERATOR_HPP_
#define MSD_CHANNEL_BLOCKING_ITERATOR_HPP_

#include <iterator>
#include <mutex>

namespace msd {

/**
 * @brief An iterator that block the current thread, waiting to fetch elements from the channel.
 *
 * Used to implement channel range-based for loop.
 *
 * @tparam Channel Instance of channel.
 */
template <typename Channel>
class blocking_iterator {
   public:
    /**
     * @brief The type of the elements stored in the channel.
     */
    using value_type = typename Channel::value_type;

    /**
     * @brief Constant reference to the type of the elements stored in the channel.
     */
    using reference = const typename Channel::value_type&;

    /**
     * @brief Constructs a blocking iterator from a channel reference.
     *
     * @param chan Reference to the channel this iterator will iterate over.
     */
    explicit blocking_iterator(Channel& chan) : chan_{chan} {}

    /**
     * @brief Advances the iterator to the next element.
     *
     * @return The iterator itself.
     */
    blocking_iterator<Channel> operator++() const noexcept { return *this; }

    /**
     * @brief Retrieves and returns the next element from the channel.
     *
     * @return A const reference to the current element.
     */
    reference operator*()
    {
        chan_ >> value_;

        return value_;
    }

    /**
     * @brief Makes iteration continue until the channel is closed and empty.
     *
     * @return true if the channel is not closed or not empty (continue iterating).
     * @return false if the channel is closed and empty (stop iterating).
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
/// \cond
template <typename T>
struct std::iterator_traits<msd::blocking_iterator<T>> {
    using value_type = typename msd::blocking_iterator<T>::value_type;
    using reference = typename msd::blocking_iterator<T>::reference;
    using iterator_category = std::input_iterator_tag;
};
/// \endcond

#endif  // MSD_CHANNEL_BLOCKING_ITERATOR_HPP_
