#include <topaz/dsa/placeholder.hpp>

using std::optional;
using std::span;
using std::nullopt;

namespace tpz::anon {
    optional<size_t> binary_search_help(span<const int> nums, int target, size_t left, size_t right) noexcept {
        if (left >= right) return nullopt;

        size_t mid = (left + right) / 2;

        if (target < nums[mid]) return binary_search_help(nums, target, left, mid);
        else if (target > nums[mid]) return binary_search_help(nums, target, mid + 1, right);
        else return mid;
    }
}

optional<size_t> tpz::dsa::binary_search(span<const int> nums, int target) noexcept {
    return tpz::anon::binary_search_help(nums, target, 0, nums.size());
}
