// Copyright (C) 2020-2025 Andrei Avram

#ifndef MSD_CHANNEL_BLOCKING_ITERATOR_HPP_
#define MSD_CHANNEL_BLOCKING_ITERATOR_HPP_

#include <cstddef>
#include <iterator>

namespace msd {

/**
 * @brief An iterator that block the current thread, waiting to fetch elements from the channel.
 *
 * Used to implement channel range-based for loop.
 *
 * @tparam Channel Type of channel being iterated.
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
     * @brief Supporting single-pass reading of elements.
     */
    using iterator_category = std::input_iterator_tag;

    /**
     * @brief Signed integral type for iterator difference.
     */
    using difference_type = std::ptrdiff_t;

    /**
     * @brief Pointer type to the value_type.
     */
    using pointer = const value_type*;

    /**
     * @brief Constructs a blocking iterator from a channel reference.
     *
     * @param chan Reference to the channel this iterator will iterate over.
     * @param is_end If true, the iterator is in an end state (no elements to read).
     */
    explicit blocking_iterator(Channel& chan, bool is_end = false) : chan_{&chan}, is_end_{is_end}
    {
        if (!is_end_ && !chan_->read(value_)) {
            is_end_ = true;
        }
    }

    /**
     * @brief Retrieves the next element from the channel.
     *
     * @return The iterator itself.
     */
    blocking_iterator<Channel>& operator++() noexcept
    {
        if (!chan_->read(value_)) {
            is_end_ = true;
        }
        return *this;
    }

    /**
     * @brief Returns the latest element retrieved from the channel.
     *
     * @return A const reference to the element.
     */
    reference operator*() { return value_; }

    /**
     * @brief Makes iteration continue until the channel is closed and empty.
     *
     * @param other Another blocking_iterator to compare with.
     *
     * @return true if the channel is not closed or not empty (continue iterating).
     * @return false if the channel is closed and empty (stop iterating).
     */
    bool operator!=(const blocking_iterator& other) { return is_end_ != other.is_end_; }

   private:
    Channel* chan_;
    value_type value_{};
    bool is_end_{false};
};

/**
 * @brief An output iterator pushes elements into a channel. Blocking until the channel is not full.
 *
 * Used to integrate with standard algorithms that require an output iterator.
 *
 * @tparam Channel Type of channel being iterated.
 */
template <typename Channel>
class blocking_writer_iterator {
   public:
    /**
     * @brief The type of the elements stored in the channel.
     */
    using value_type = typename Channel::value_type;

    /**
     * @brief Constant reference to the type of the elements stored in the channel.
     */
    using reference = const value_type&;

    /**
     * @brief Supporting writing of elements.
     */
    using iterator_category = std::output_iterator_tag;

    /**
     * @brief Signed integral type for iterator difference.
     */
    using difference_type = std::ptrdiff_t;

    /**
     * @brief Pointer type to the value_type.
     */
    using pointer = const value_type*;

    /**
     * @brief Constructs a blocking iterator from a channel reference.
     *
     * @param chan Reference to the channel this iterator will write into.
     */
    explicit blocking_writer_iterator(Channel& chan) : chan_{&chan} {}

    /**
     * @brief Writes an element into the channel, blocking until space is available.
     *
     * @note There is no effect if the channel is closed.
     *
     * @param value The value to be written into the channel.
     *
     * @return The iterator itself.
     */
    blocking_writer_iterator& operator=(reference value)
    {
        chan_->write(value);
        return *this;
    }

    /**
     * @brief Not applicable (handled by operator=).
     *
     * @note It's uncommon to return a reference to an iterator, but I don't want to return a value from the channel.
     * This iterator is supposed to be used only to write values.
     * I don't know if it's a terrible idea or not, but it looks related to the issue with MSVC
     * in the Transform test in tests/channel_test.cpp.
     *
     * @return The iterator itself.
     */
    blocking_writer_iterator& operator*() { return *this; }

    /**
     * @brief Not applicable (handled by operator=).
     *
     * @return The iterator itself.
     */
    blocking_writer_iterator& operator++() { return *this; }

    /**
     * @brief Not applicable (handled by operator=).
     *
     * @return The iterator itself.
     */
    blocking_writer_iterator operator++(int) { return *this; }

   private:
    Channel* chan_;
};

/**
 * @brief Creates a blocking iterator for the given channel.
 *
 * @tparam Channel Type of channel being iterated.
 *
 * @param chan Reference to the channel this iterator will iterate over.
 *
 * @return A blocking iterator for the specified channel.
 */
template <typename Channel>
blocking_writer_iterator<Channel> back_inserter(Channel& chan)
{
    return blocking_writer_iterator<Channel>{chan};
}

}  // namespace msd

#endif  // MSD_CHANNEL_BLOCKING_ITERATOR_HPP_
