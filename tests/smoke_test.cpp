#include <gtest/gtest.h>

#include <topaz/dsa/placeholder.hpp>
#include <topaz/engine/placeholder.hpp>
#include <topaz/net/placeholder.hpp>

#include <array>

using std::nullopt;

TEST(Smoke, BuildsAndRuns) { EXPECT_EQ(1 + 1, 2); }

TEST(Smoke, LinksAgainstTopaz) {
  EXPECT_EQ(tpz::dsa::placeholder(), 0);
  EXPECT_EQ(tpz::eng::placeholder(), 0);
  EXPECT_EQ(tpz::net::placeholder(), 0);
}
