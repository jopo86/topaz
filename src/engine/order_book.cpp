#include <topaz/engine/order_book.hpp>
#include <topaz/types.hpp>

namespace tpz::eng {

OrderBook::OrderBook() {}

void OrderBook::submit(Request request) {
  std::visit(overloaded {
    [&](NewLimit order) {
      // TODO:
    },
    [&](NewMarket order) {
      // TODO:
    },
    [&](Cancel cancel) {
      // TODO:
    },
  }, request);
}

} // namespace tpz::eng
