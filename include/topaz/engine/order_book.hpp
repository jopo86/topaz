#pragma once

#include "../types.hpp"
#include "event.hpp"

#include <map>
#include <unordered_map>
#include <list>

namespace tpz::eng {

class OrderBook {
public:
  OrderBook();

  void submit(Request request);

private:
  // TODO: set sorting order
  std::unordered_map<OrderId, RestingOrder> orders_;
  std::map<Price, std::list<OrderId>> buy_orders_;
  std::map<Price, std::list<OrderId>> sell_orders_;
  std::list<Event> events_;
};

} // namespace tpz::eng
