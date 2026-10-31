#pragma once

#include "../types.hpp"
#include "event.hpp"

#include <map>
#include <unordered_map>
#include <list>
#include <utility>

namespace tpz::eng {

template<EventSink Sink>
class OrderBook {
public:
  explicit OrderBook(Sink& sink) : sink_(sink) {}

  void submit(Request request) {
    std::visit(overloaded {
      [&](NewLimit order) {
        sink_.on(Accepted { request });
        match(order);
      },
      [&](NewMarket order) {
        sink_.on(Accepted { request });
        match(order);
      },
      [&](Cancel c) {
        sink_.on(Accepted { request });
        cancel(c.id);
      },
    }, request);
  }

private:
  void match(NewLimit order) {
    if (order.side == Side::Ask) {
      // TODO: support FillOrKill orders
      // Look at orders at each price level, descending
      for (auto it_p = buy_orders_.begin(); it_p != buy_orders_.end();) {
        
        Price price = it_p->first;
        std::list<OrderId>& orders = it_p->second;

        if (price < order.price) break;
        
        // Look at the orders at this price level, ordered by recency (FIFO)
        for (auto it_o = orders.begin(); it_o != orders.end();) {
          OrderId id = *it_o;
          RestingOrder resting_order = orders_[id];
          if (resting_order.qty >= order.qty) {
            // order filled, resting order partially filled
            sink_.on(Filled { price, order.qty });
            resting_order.qty -= order.qty;
            it_o++;
          }
          else {
            // order partially filled, resting order filled
            sink_.on(Filled { price, resting_order.qty });
            order.qty -= resting_order.qty;
            it_o = orders.erase(orders.begin());
          }
        }
        
        it_p++; // only if we didn't erase
      }

    }
    else if (order.side == Side::Bid) {
      for (const auto& [price, orders] : sell_orders_) { // sorted by ascending price
        if (price > order.price) break;
        for (OrderId id : orders) {
          // TODO: fill both sides
        }
      }
    }
    else {
      std::unreachable();
    }
  }

  void match(NewMarket order) {

  }

  void cancel(OrderId order) {

  }

  // TODO: set sorting order
  Sink& sink_;
  std::unordered_map<OrderId, RestingOrder> orders_;
  std::map<Price, std::list<OrderId>, std::greater<Price>> buy_orders_;
  std::map<Price, std::list<OrderId>> sell_orders_;
  std::list<Event> events_;
};

} // namespace tpz::eng
