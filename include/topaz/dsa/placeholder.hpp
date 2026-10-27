#pragma once

#include <span>
#include <optional>

// Placeholder so the topaz library target has something to build.
// Delete once real tpz::dsa code lands.

namespace tpz::dsa {

    [[nodiscard]] std::optional<std::size_t> binary_search(std::span<const int> nums, int target) noexcept;


} // namespace tpz::dsa
