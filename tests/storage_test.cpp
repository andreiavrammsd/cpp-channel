#include "msd/storage.hpp"

#include <gtest/gtest.h>

#include <memory>

template <typename Storage>
class StorageTest : public ::testing::Test {};

using StorageTypes = ::testing::Types<msd::queue_storage<int>, msd::vector_storage<int>, msd::array_storage<int, 10>>;

TYPED_TEST_SUITE(StorageTest, StorageTypes, );

TYPED_TEST(StorageTest, PushAndPop)
{
    TypeParam storage{10};

    int in = 42;
    storage.push_back(in);
    storage.push_back(99);
    EXPECT_EQ(storage.size(), 2);

    int out;
    storage.pop_front(out);
    EXPECT_EQ(out, 42);

    storage.pop_front(out);
    EXPECT_EQ(out, 99);

    EXPECT_EQ(storage.size(), 0);
}

template <typename Storage>
class StorageWithMovableOnlyTypeTest : public ::testing::Test {};

using StorageWithMovableOnlyTypeTypes =
    ::testing::Types<msd::queue_storage<std::unique_ptr<int>>, msd::vector_storage<std::unique_ptr<int>>,
                     msd::array_storage<std::unique_ptr<int>, 10>>;

TYPED_TEST_SUITE(StorageWithMovableOnlyTypeTest, StorageWithMovableOnlyTypeTypes, );

TYPED_TEST(StorageWithMovableOnlyTypeTest, PushAndPop)
{
    TypeParam storage{10};

    storage.push_back(std::make_unique<int>(123));
    EXPECT_EQ(storage.size(), 1);

    std::unique_ptr<int> out;
    storage.pop_front(out);

    ASSERT_TRUE(out);
    EXPECT_EQ(*out, 123);
}

TEST(ArrayStorageTest, Capacity) { EXPECT_EQ((msd::array_storage<int, 10>::capacity), 10); }
