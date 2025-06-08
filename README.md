# Channel

[![build](https://github.com/andreiavrammsd/cpp-channel/workflows/build/badge.svg)](https://github.com/andreiavrammsd/cpp-channel/actions) [![codecov](https://codecov.io/github/andreiavrammsd/cpp-channel/graph/badge.svg?token=CKQ0TVW62Z)](https://codecov.io/github/andreiavrammsd/cpp-channel)
[![documentation](https://github.com/andreiavrammsd/cpp-channel/workflows/doc/badge.svg)](https://andreiavrammsd.github.io/cpp-channel/)

### Thread-safe container for sharing data between threads. Header-only.

* Thread-safe push and fetch.
* Use stream operators to push (<<) and fetch (>>) items.
* Value type must be default constructible.
* Blocking (forever waiting to fetch).
* Range-based for loop supported.
* Close to prevent pushing and stop waiting to fetch.
* Integrates well with STL algorithms in some cases. Eg: std::move(ch.begin(), ch.end(), ...).
* Tested with GCC, Clang, and MSVC.

## Requirements

* C++11 or newer

## Installation

Choose one of the methods:

* Copy the [include](https://github.com/andreiavrammsd/cpp-channel/tree/master/include) directory into your project and add it to your include path.
* [CMake FetchContent](https://github.com/andreiavrammsd/cpp-channel/tree/master/examples/cmake-project)
* [CMake install](https://cmake.org/cmake/help/latest/command/install.html)
```shell
VERSION=1.0.1 \
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
    msd::channel<int> chan{2}; // buffered

    // Send to channel
    chan << 1;
    chan << 2;
    chan << 3; // blocking because capacity is 2 (and no one reads from channel)
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
    chan >> out; // blocking because channel is empty (and no one writes on it)
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

    for (const auto out : chan) { // blocking: forever waiting for channel items
        std::cout << out << '\n';
    }
}
```

See [examples](https://github.com/andreiavrammsd/cpp-channel/tree/master/examples) and [documentation](https://andreiavrammsd.github.io/cpp-channel/).

<br>

Developed with [CLion](https://www.jetbrains.com/?from=serializer) and [Visual Studio Code](https://code.visualstudio.com/).
