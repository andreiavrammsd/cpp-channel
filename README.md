# Channel

**Not tested in production**

Thread safe communication object. Tested with GCC and Clang.

## Requirements
* C++11/14/17
* CMake 3.12+
* pthread

## Usage

```c++
#include <cassert>

#include "channel.h"

int main() {
    Channel<int> channel; // unbuffered

    int in = 1;
    int out = 0;

    // Send to channel
    in >> channel;

    // Read from channel
    out << channel;

    assert(out == 1);
}
```

```c++
#include "channel.h"

int main() {
    Channel<int> channel{2}; // buffered

    int in = 1;

    // Send to channel
    in >> channel;
    in >> channel;
    in >> channel; // blocking because capacity is 2 (and no one reads from channel)
}
```

```c++
#include "channel.h"

int main() {
    Channel<int> channel{2}; // buffered

    int in = 1;
    int out = 0;

    // Send to channel
    in >> channel;
    in >> channel;

    // Read from channel
    out << channel;
    out << channel;
    out << channel; // blocking because channel is empty (and no one writes on it)
}
```

```c++
#include <iostream>

#include "channel.h"

int main() {
    Channel<int> channel;

    int in1 = 1;
    in1 >> channel;

    int in2 = 2;
    in2 >> channel;

    for (const auto out : channel) {  // blocking: forever waiting for channel items
        std::cout << out << '\n';
    }
}
```

See [example app](example.cpp).