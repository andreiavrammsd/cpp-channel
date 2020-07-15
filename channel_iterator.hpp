#ifndef CHANNEL_ITERATOR_HPP_
#define CHANNEL_ITERATOR_HPP_

/**
 * Blocking infinite iterator
 *
 * Used to implement channel range-based for loop
 *
 * @tparam T
 */
template <typename T>
class Channel;

template <typename T>
class channel_iterator {
   public:
    explicit channel_iterator(Channel<T>& ch) : ch{ch} {}

    channel_iterator<T> operator++() { return *this; }

    T operator*()
    {
        T value{};
        value << ch;

        return value;
    }

    bool operator!=(channel_iterator<T>) { return true; }

   private:
    Channel<T>& ch;
};

#endif  // CHANNEL_ITERATOR_HPP_
