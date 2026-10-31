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

template<typename S>
concept EventSink = requires (S& s, const Accepted& a, const Filled& f,
                              const Cancelled& c, const Rejected& r) {
  s.on(a); s.on(f); s.on(c); s.on(r);
};

} // namespace tpz::eng
