# Channel

[![build](https://github.com/andreiavrammsd/cpp-channel/workflows/build/badge.svg)](https://github.com/andreiavrammsd/cpp-channel/actions)

### Thread-safe container for sharing data between threads. Header-only.

* Thread-safe push and fetch.
* Use stream operators to push (>>) and fetch (<<) items.
* Blocking (forever waiting to fetch).
* Range-based for loop supported.
* Close to prevent pushing and stop waiting to fetch.
* Integrates well with STL algorithms. Eg: std::move(ch.begin(), ch.end(), ...).
* Tested with GCC and Clang.

## Requirements

* C++11 or newer

## Installation

Copy the [include](./include) directory to your project and add it to your include path. Or
see [CMakeLists.txt](./examples/cmake-project/CMakeLists.txt) from the [CMake project example](./examples/cmake-project)
.

## Usage

```c++
#include <cassert>

#include <msd/channel.hpp>

int main() {
    msd::channel<int> chan; // unbuffered

    int in = 1;
    int out = 0;

    // Send to channel
    in >> chan;

    // Read from channel
    out << chan;

    assert(out == 1);
}
```

```c++
#include <msd/channel.hpp>

int main() {
    msd::channel<int> chan{2}; // buffered

    int in = 1;

    // Send to channel
    in >> chan;
    in >> chan;
    in >> chan; // blocking because capacity is 2 (and no one reads from channel)
}
```

```c++
#include <msd/channel.hpp>

int main() {
    msd::channel<int> chan{2}; // buffered

    int in = 1;
    int out = 0;

    // Send to channel
    in >> chan;
    in >> chan;

    // Read from channel
    out << chan;
    out << chan;
    out << chan; // blocking because channel is empty (and no one writes on it)
}
```

```c++
#include <iostream>

#include <msd/channel.hpp>

int main() {
    msd::channel<int> chan;

    int in1 = 1;
    in1 >> chan;

    int in2 = 2;
    in2 >> chan;

    for (const auto out : chan) {  // blocking: forever waiting for channel items
        std::cout << out << '\n';
    }
}
```

See [examples](examples).

<br>

Developed with [CLion](https://www.jetbrains.com/?from=serializer)

<a href="https://www.jetbrains.com/?from=serializer">![JetBrains](jetbrains.svg)</a>
