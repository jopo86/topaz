#pragma once

#include "../types.hpp"

namespace tpz::eng {

struct Accepted {
  Request request;
};

struct Filled {
  Price price;
  Qty qty;
};

struct Cancelled {
  OrderId id;
};

struct Rejected {
  enum class Reason {
    Placeholder,
  };
  OrderId id;
  Reason reason;
};

using Event = std::variant<Accepted, Filled, Cancelled, Rejected>;

} // namespace tpz::eng
