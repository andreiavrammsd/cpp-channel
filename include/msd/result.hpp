// Copyright (C) 2020-2025 Andrei Avram

#ifndef MSD_CHANNEL_RESULT_HPP_
#define MSD_CHANNEL_RESULT_HPP_

/** @file */

namespace msd {

/**
 * @brief A result that contains either a value of type T or an error of type E.
 *
 * @tparam T The type of the value on success.
 * @tparam E The type of the error on failure.
 */
template <typename T, typename E>
class result {
   public:
    /**
     * @brief Constructs an empty result (not valid).
     */
    explicit result() = default;

    /**
     * @brief Constructs a successful result with a value.
     *
     * @param value The value to store into the result.
     */
    explicit result(T value) : value_{value} {}

    /**
     * @brief Constructs an error result.
     *
     * @param error The error value to store.
     */
    explicit result(E error) : has_error_{true}, error_{error} {}

    /**
     * @brief Checks whether the result is a success.
     *
     * @return true if the result holds a value, false if it holds an error.
     */
    explicit operator bool() const { return !has_error_; }

    /**
     * @brief Gets the stored value.
     *
     * @return const T& Reference to the stored value.
     * @warning Behavior is undefined if the result holds an error.
     */
    const T& value() const { return value_; }

    /**
     * @brief Gets the stored error.
     *
     * @return const E& Reference to the stored error.
     * @warning Behavior is undefined if the result holds a value.
     */
    const E& error() const { return error_; }

   private:
    union {
        T value_;
        E error_;
    };
    bool has_error_{};
};

/**
 * @brief Specialization of result for void success type.
 *
 * @tparam E The type of the error on failure.
 */
template <typename E>
class result<void, E> {
   public:
    /**
     * @brief Constructs a successful void result.
     */
    result() = default;

    /**
     * @brief Constructs an error result.
     *
     * @param error The error value to store.
     */
    explicit result(E error) : has_value_{false}, error_{error} {}

    /**
     * @brief Checks whether the result is a success.
     *
     * @return true if the result is successful, false otherwise.
     */
    explicit operator bool() const { return has_value_; }

    /**
     * @brief Gets the stored error.
     *
     * @return Const reference to the stored error.
     * @note Only valid if the result holds an error.
     */
    const E& error() const { return error_; }

   private:
    bool has_value_{true};
    E error_;
};

}  // namespace msd

#endif  // MSD_CHANNEL_RESULT_HPP_
