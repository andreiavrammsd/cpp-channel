// Copyright (C) 2022 Andrei Avram

#ifndef MSD_CHANNEL_BLOCKING_ITERATOR_HPP_
#define MSD_CHANNEL_BLOCKING_ITERATOR_HPP_

#include <iterator>
#include <memory>
#include <mutex>
#include <type_traits>

namespace msd {

/**
 * @brief An iterator that block the current thread,
 * waiting to fetch elements from the channel.
 *
 * Used to implement channel range-based for loop.
 *
 * @tparam Channel Instance of channel.
 */
template <typename channel>
class blocking_iterator {
   public:
    using value_type = typename channel::value_type;

    blocking_iterator() : ch_(nullptr) {}
    explicit blocking_iterator(channel& ch) : ch_{&ch} {}

    /**
     * Advances to next element in the channel.
     */
    blocking_iterator<channel>& operator++() noexcept
    {
        assert(ch_);
        if (!value_)
            value_.reset(new value_type);
        (*value_) << (*ch_);
        return *this;
    }

    /**
     * Returns an element from the channel.
     */
    value_type& operator*()
    {
        if (!value_)
            ++(*this);
        return *value_;
    }

    /**
     * Makes iteration continue until the channel is closed and empty.
     */
    bool operator!=(const blocking_iterator<channel>& ch) const
    {
        if (ch_ == ch.ch_ || ch.ch_)
            return false;
        return !(ch_->closed() && ch_->empty());
    }

   private:
    channel* ch_;
    std::shared_ptr<value_type> value_;
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
