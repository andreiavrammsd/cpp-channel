// Copyright (C) 2020-2025 Andrei Avram

#ifndef MSD_CHANNEL_STATIC_CHANNEL_HPP_
#define MSD_CHANNEL_STATIC_CHANNEL_HPP_

#include <cstdlib>

#include "channel.hpp"

/** @file */

namespace msd {

/**
 * @brief Thread-safe container for sharing data between threads.
 *
 * - Allocates elements on the stack.
 * - Does not throw exceptions.
 * - Not movable, not copyable.
 * - Includes a blocking input iterator.
 * - Always buffered (with **Capacity**).
 *
 * @tparam T The type of the elements.
 * @tparam Capacity The maximum number of elements the channel can hold before blocking. Must be greater than zero.
 */
template <typename T, std::size_t Capacity>
using static_channel = channel<T, array_storage<T, Capacity>>;

}  // namespace msd

#endif  // MSD_CHANNEL_STATIC_CHANNEL_HPP_
