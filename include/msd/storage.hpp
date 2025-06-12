// Copyright (C) 2020-2025 Andrei Avram

#ifndef MSD_CHANNEL_STORAGE_HPP_
#define MSD_CHANNEL_STORAGE_HPP_

#include <array>
#include <cstdlib>
#include <queue>
#include <vector>

#include "nodiscard.hpp"

/** @file */

namespace msd {

/**
 * @brief A FIFO queue storage using std::queue.
 *
 * @tparam T Type of elements stored.
 */
template <typename T>
class queue_storage {
   public:
    /**
     * @brief Constructs the queue storage (parameter ignored, required for interface compatibility).
     */
    explicit queue_storage(std::size_t) {}

    /**
     * @brief Adds an element to the back of the queue.
     *
     * @tparam Type Type of the element to insert.
     * @param value The value to insert (perfect forwarded).
     */
    template <typename Type>
    void push_back(Type&& value)
    {
        queue_.push(std::forward<Type>(value));
    }

    /**
     * @brief Removes the front element from the queue and moves it to the output.
     *
     * @param out Reference to the variable where the front element will be moved.
     * @note It's undefined behaviour to pop from an empty queue.
     */
    void pop_front(T& out)
    {
        out = std::move(queue_.front());
        queue_.pop();
    }

    /**
     * @brief Returns the number of elements currently stored.
     *
     * @return Current size.
     */
    NODISCARD std::size_t size() const noexcept { return queue_.size(); }

   private:
    std::queue<T> queue_;
};

/**
 * @brief A FIFO queue storage using std::vector.
 *
 * @tparam T Type of elements stored.
 */
template <typename T>
class vector_storage {
   public:
    /**
     * @brief Constructs a queue storage with a given capacity.
     *
     * @param capacity Maximum number of elements the storage can hold.
     * @note Reserves the memory up-front.
     */
    explicit vector_storage(std::size_t capacity) { vector_.reserve(capacity); }

    /**
     * @brief Adds an element to the back of the vector.
     *
     * @tparam Type Type of the element to insert.
     * @param value The value to insert (perfect forwarded).
     */
    template <typename Type>
    void push_back(Type&& value)
    {
        vector_.push_back(std::forward<Type>(value));
    }

    /**
     * @brief Removes the front element from the vector and moves it to the output.
     *
     * @param out Reference to the variable where the front element will be moved.
     * @note It's undefined behaviour to pop from an empty vector.
     */
    void pop_front(T& out)
    {
        out = std::move(vector_.front());
        vector_.erase(vector_.begin());
    }

    /**
     * @brief Returns the number of elements currently stored.
     *
     * @return Current size.
     */
    NODISCARD std::size_t size() const noexcept { return vector_.size(); }

   private:
    std::vector<T> vector_;
};

/**
 * @brief A fixed-size circular buffer using std::array.
 *
 * @tparam T Type of elements stored.
 * @tparam N Maximum number of elements (capacity).
 */
template <typename T, std::size_t N>
class array_storage {
   public:
    static_assert(N > 0, "Capacity must be greater than zero.");

    /**
     * @brief The storage capacity.
     *
     * @note Required for static storage.
     */
    static constexpr std::size_t capacity = N;

    /**
     * @brief Constructs the array storage (parameter ignored, required for interface compatibility).
     */
    explicit array_storage(std::size_t) {}

    /**
     * @brief Adds an element to the back of the array.
     *
     * @tparam Type Type of the element to insert.
     * @param value The value to insert (perfect forwarded).
     */
    template <typename Type>
    void push_back(Type&& value)
    {
        array_[(front_ + size_) % N] = std::forward<Type>(value);
        ++size_;
    }

    /**
     * @brief Marks the front element as removed and moves it to the output.
     *
     * @param out Reference to the variable where the front element will be moved.
     * @note It's undefined behaviour to pop from an empty array.
     */
    void pop_front(T& out)
    {
        out = std::move(array_[front_]);
        front_ = (front_ + 1) % N;
        --size_;
    }

    /**
     * @brief Returns the number of elements currently stored.
     *
     * @return Current size.
     */
    NODISCARD std::size_t size() const noexcept { return size_; }

   private:
    std::array<T, N> array_{};
    std::size_t size_{0};
    std::size_t front_{0};
};

}  // namespace msd

#endif  // MSD_CHANNEL_STORAGE_HPP_
