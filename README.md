# Channel

**Under development**

Thread safe communication object.

## Requirements
* C++11
* CMake 3.12+

## Installation
TBD

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

See [example app](example.cpp).