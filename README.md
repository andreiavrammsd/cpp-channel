# Channel

[![build](https://github.com/andreiavrammsd/cpp-channel/actions/workflows/cmake.yml/badge.svg)](https://github.com/andreiavrammsd/cpp-channel/actions) [![codecov](https://codecov.io/github/andreiavrammsd/cpp-channel/graph/badge.svg?token=CKQ0TVW62Z)](https://codecov.io/github/andreiavrammsd/cpp-channel)
[![documentation](https://github.com/andreiavrammsd/cpp-channel/actions/workflows/doc.yml/badge.svg)](https://andreiavrammsd.github.io/cpp-channel/)

### Thread-safe container for sharing data between threads (synchronized queue). Header-only. Compatible with C++11 and newer.

## About

`msd::channel`
* A synchronized queue that can be easily and safely shared between multiple threads.
* Tested with GCC, Clang, and MSVC.
* Uses [std::mutex](https://en.cppreference.com/w/cpp/thread/mutex.html) for synchronization.
* Uses a customizable `storage` to store elements.

It's a class that can be constructed in several ways:
* Buffered:
    * The channel accepts a specified number of elements, after which it blocks the writer threads and waits for a reader thread to read an element.
    * It blocks the reader threads when channel is empty until a writer thread writes elements.
    * `msd::channel<int> chan{2};`
* Unbuffered:
    * Never blocks writes.
    * It blocks the reader threads when channel is empty until a writer thread writes elements.
    * `msd::channel<int> chan{};`
* Heap- or stack-allocated: pass a custom storage or choose a [built-in storage](https://github.com/andreiavrammsd/cpp-channel/blob/master/include/msd/storage.hpp):
    * `msd::queue_storage` (default): uses [std::queue](https://en.cppreference.com/w/cpp/container/queue.html)
    * `msd::vector_storage`: uses [std::vector](https://en.cppreference.com/w/cpp/container/vector.html) (if cache locality is important)
        * `msd::channel<int, msd::vector_storage<int>> chan{2};`
    * `msd::array_storage` (always buffered): uses [std::array](https://en.cppreference.com/w/cpp/container/array.html) (if you want stack allocation)
        * `msd::channel<int, msd::array_storage<int, 10>> chan{};`
        * `msd::channel<int, msd::array_storage<int, 10>> chan{10}; // does not compile because capacity is already passed as template argument`
        * aka `msd::static_channel<int, 10>`

A `storage` is:
* A class with a specific interface for storing elements.
* Must implement [FIFO](https://en.wikipedia.org/wiki/FIFO) logic.
* See [built-in storages](https://github.com/andreiavrammsd/cpp-channel/blob/master/include/msd/storage.hpp).

Exceptions:
* msd::operator<< throws `msd::closed_channel` if channel is closed.
* `msd::channel::write` returns `bool` status instead of throwing.
* Heap-allocated storages could throw.
* Static-allocated storage does not throw.
* Throws if stored elements throw.

## Features

* Thread-safe push and fetch.
* Use stream operators to push (<<) and fetch (>>) items.
* Value type must be default constructible.
* Blocking (forever waiting to fetch).
* Range-based for loop supported.
* Close to prevent pushing and stop waiting to fetch.
* Integrates with some of the STL algorithms. Eg:
    * `std::move(ch.begin(), ch.end(), ...)`
    * `std::transform(input_chan.begin(), input_chan.end(), msd::back_inserter(output_chan))`.
    * `std::copy_if(chan.begin(), chan.end(), ...);`

## Installation

Choose one of the methods:

* Copy the [include](https://github.com/andreiavrammsd/cpp-channel/tree/master/include) directory into your project and add it to your include path.
* [CMake FetchContent](https://github.com/andreiavrammsd/cpp-channel/tree/master/examples/cmake-project)
* [CMake install](https://cmake.org/cmake/help/latest/command/install.html) - choose a [version](https://github.com/andreiavrammsd/cpp-channel/releases), then run:
```shell
VERSION=X.Y.Z \
    && wget https://github.com/andreiavrammsd/cpp-channel/archive/refs/tags/v$VERSION.zip \
    && unzip v$VERSION.zip \
    && cd cpp-channel-$VERSION \
    && mkdir build && cd build \
    && cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local \
    && sudo cmake --install .
```
* [Bazel](https://github.com/andreiavrammsd/cpp-channel/tree/master/examples/bazel-project)

## Usage

```c++
#include <cassert>

#include <msd/channel.hpp>

int main() {
    msd::channel<int> chan; // unbuffered

    int in = 1;
    int out = 0;

    // Send to channel
    chan << in;

    // Read from channel
    chan >> out;

    assert(out == 1);
}
```

```c++
#include <msd/channel.hpp>

int main() {
    msd::channel<int, msd::vector_storage<int>> chan{2}; // buffered

    // Send to channel
    chan << 1;
    chan << 2;
    chan << 3; // blocks because capacity is 2 (and no one reads from channel)
}
```

```c++
#include <msd/channel.hpp>

int main() {
    msd::channel<int> chan{2}; // buffered

    int in = 1;
    int out = 0;

    // Send to channel
    chan << in;
    chan << in;

    // Read from channel
    chan >> out;
    chan >> out;
    chan >> out; // blocks because channel is empty (and no one writes on it)
}
```

```c++
#include <iostream>

#include <msd/channel.hpp>

int main() {
    msd::channel<int> chan;

    int in1 = 1;
    int in2 = 2;

    chan << in1 << in2;

    for (const auto out : chan) { // blocks: waits forever for channel items
        std::cout << out << '\n';
    }
}
```

```c++
#include <msd/static_channel.hpp>

int main() {
    msd::static_channel<int, 2> chan{}; // always buffered

    int in = 1;
    int out = 0;

    // Send to channel
    chan.write(in);
    chan.write(in);

    // Read from channel
    chan.read(out);
    chan.read(out);
    chan.read(out); // blocks because channel is empty (and no one writes on it)
}
```

See [examples](https://github.com/andreiavrammsd/cpp-channel/tree/master/examples) and [documentation](https://andreiavrammsd.github.io/cpp-channel/).

## Known limitations

* In some cases, the integration with some STL algorithms does not compile with MSVC. See the [Transform test](https://github.com/andreiavrammsd/cpp-channel/blob/master/tests/channel_test.cpp).

<br>

Developed with [CLion](https://www.jetbrains.com/?from=serializer) and [Visual Studio Code](https://code.visualstudio.com/).
