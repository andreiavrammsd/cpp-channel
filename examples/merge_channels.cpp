#include <algorithm>
#include <chrono>
#include <future>
#include <iostream>
#include <sstream>
#include <thread>

#include "msd/channel.hpp"

int main()
{
    msd::channel<int> input_chan{30};
    msd::channel<int> output_chan{10};

    // Send to channel
    const auto writer = [&input_chan](int begin, int end) {
        for (int i = begin; i <= end; ++i) {
            input_chan.write(i);

            std::stringstream msg;
            msg << "Sent " << i << " from " << std::this_thread::get_id() << "\n";
            std::cout << msg.str();

            std::this_thread::sleep_for(std::chrono::milliseconds(10));  // simulate work
        }
        input_chan.close();
    };

    const auto reader = [&output_chan]() {
        for (const auto out : output_chan) {  // blocking until channel is drained (closed and empty)
            std::stringstream msg;
            msg << "Received " << out << " on " << std::this_thread::get_id() << "\n";
            std::cout << msg.str();

            std::this_thread::sleep_for(std::chrono::milliseconds(200));  // simulate work
        }
    };

    const auto transformer = [&input_chan, &output_chan]() {
        const auto double_value = [](int value) { return value * 2; };

#ifdef _MSC_VER
        for (auto&& value : input_chan) {
            output_chan.write(double_value(value));
        }

        // Does not work with std::transform:
        //
        // could be 'void std::queue<T,std::deque<T,std::allocator<int>>>::push(int &&)'
        //   with
        //   [
        //       T=ChannelTest_Traits_Test::TestBody::type
        //   ]
        //   D:\a\cpp-channel\cpp-channel\include\msd\channel.hpp(105,20):
        //   'void std::queue<T,std::deque<T,std::allocator<int>>>::push(int &&)': cannot convert argument 1 from
        //   '_OutIt' to 'int &&' with
        //   [
        //       T=ChannelTest_Traits_Test::TestBody::type
        //   ]
        //   and
        //   [
        //       _OutIt=msd::blocking_writer_iterator<msd::channel<ChannelTest_Traits_Test::TestBody::type>>
        //   ]
        //       D:\a\cpp-channel\cpp-channel\include\msd\channel.hpp(105,43):
        //       Reason: cannot convert from '_OutIt' to 'int'
        //   with
        //   [
        //       _OutIt=msd::blocking_writer_iterator<msd::channel<ChannelTest_Traits_Test::TestBody::type>>
        //   ]
#else
        std::transform(input_chan.begin(), input_chan.end(), msd::back_inserter(output_chan), double_value);
#endif  // _MSC_VER

        output_chan.close();
    };

    const auto reader_1 = std::async(std::launch::async, reader);
    const auto reader_2 = std::async(std::launch::async, reader);
    const auto writer_1 = std::async(std::launch::async, writer, 1, 30);
    const auto writer_2 = std::async(std::launch::async, writer, 31, 40);
    const auto transformer_task = std::async(std::launch::async, transformer);

    reader_1.wait();
    reader_2.wait();
    writer_1.wait();
    writer_2.wait();
    transformer_task.wait();
}
