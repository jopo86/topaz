#pragma once
#include <cassert>

#ifndef NDEBUG
  #define TPZ_ASSERT(cond) assert(cond)
#elif defined(_MSC_VER) && !defined(__clang__)
  #define TPZ_ASSERT(cond) __assume(cond)
#else
  #define TPZ_ASSERT(cond) [[assume(cond)]]
#endif
