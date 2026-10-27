#include <gtest/gtest.h>

#include <topaz/dsa/placeholder.hpp>
#include <topaz/engine/placeholder.hpp>
#include <topaz/net/placeholder.hpp>

#include <array>

using std::nullopt;

TEST(Smoke, BinarySearch) {
    std::array<int, 9> arr = { 1, 2, 3, 4, 5, 7, 8, 9, 10 };
    EXPECT_EQ(tpz::dsa::binary_search(arr, 2), 1u);
    EXPECT_EQ(tpz::dsa::binary_search(arr, 5), 4u);
    EXPECT_EQ(tpz::dsa::binary_search(arr, 10), 8u);
    EXPECT_EQ(tpz::dsa::binary_search(arr, 0), nullopt);
    EXPECT_EQ(tpz::dsa::binary_search(arr, 11), nullopt);
    EXPECT_EQ(tpz::dsa::binary_search(arr, 6), nullopt);
}
