#pragma once

#include <cstdint>
#include <variant>

namespace tpz {

template<typename Tag>
class StrongInt {
public:
  using Rep = std::int64_t;

  constexpr StrongInt() : val_(0) {};
  constexpr explicit StrongInt(Rep val) : val_(val) {}

  [[nodiscard]] constexpr Rep val() const { return val_; }

  friend constexpr auto operator<=>(StrongInt, StrongInt) = default;

private:
  Rep val_;
};

struct PriceTag {};
struct QtyTag {};
struct OrderIdTag {};

using Price   = StrongInt<PriceTag>;
using Qty     = StrongInt<QtyTag>;
using OrderId = StrongInt<OrderIdTag>;

enum class Side {
  Bid,
  Ask,
};

enum class Tif {
  GoodTillCancel,
  ImmediateOrCancel,
  FillOrKill,
};

struct NewLimit {
  OrderId id;
  Side side;
  Price price;
  Qty qty;
  Tif tif;
};

struct NewMarket {
  OrderId id;
  Side side;
  Qty qty;
  Tif tif;
};

struct Cancel {
  OrderId id;
};

using Request = std::variant<NewLimit, NewMarket, Cancel>;

struct RestingOrder {
  OrderId id;
  Side side;
  Price price;
  Qty qty;
};

} // namespace tpz
