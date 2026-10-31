# Design decisions

## `tpz::dsa::List`

### Forward-owning nodes (2026-10-28)

**Decision:** Each node owns the next one. `head_` and `next_` are `std::unique_ptr`; `tail_` and `prev_` are raw, non-owning pointers.

**Reasoning:** A doubly linked list has two links into each node, but only one of them can own it. Owning in the forward direction keeps memory management in the types: no manual `delete`, and moves come almost for free.

**Alternatives considered:**
- *All `unique_ptr`*: impossible, because every node would have two unique owners (the previous node's `next` and the next node's `prev`). In practice that means double frees, or not compiling at all.
- *All raw pointers*: works, but every insert, erase, move, and the destructor need hand-written `new`/`delete`, which makes leaks and double frees easy to introduce.

### Iterative destruction (2026-10-29)

**Decision:** `clear()` frees nodes one at a time in a loop. The destructor and move assignment both call it.

**Reasoning:** With forward ownership, letting `head_` go out of scope destroys the chain recursively: each node's `next_` destructor frees the next node. That is one stack frame per element, so a long list overflows the stack. The loop cuts each `next_` link before the node is freed, so stack depth stays constant.

**Alternatives considered:**
- *Recursive (default) destruction*: no code needed, but stack depth grows with the list length. `LongListDestroysWithoutStackOverflow` tests that we avoid this.

## Core Shared Types

### Shared types live in `tpz`, in `topaz/types.hpp` (2026-10-30)

**Decision:** `Price`, `Qty`, `OrderId`, `Side`, and the request types live directly in namespace `tpz`, in `include/topaz/types.hpp`.

**Reasoning:** These types are the vocabulary of the whole system, not engine internals. `tpz::net` needs them to encode and decode order entry and market data messages, and the GUI needs them to display and submit orders. Keeping them out of `tpz::eng` means neither has to depend on the engine just to name a price.

**Alternatives considered:**
- *`tpz::eng` in `topaz/engine/types.hpp`*: also workable, since `net` depending on `eng` is the allowed direction. Rejected because anything that needs a `Price` would then depend on the engine.
- *`defs.hpp` / `common.hpp`*: vague names like these tend to collect unrelated macros and helpers over time. Headers are named after what they contain, like `assert.hpp`.

### `StrongInt` has an `explicit` constructor (2026-10-30)

**Decision:** `StrongInt`'s constructor from `Rep` is `explicit`, so raw integers don't convert implicitly to `Price`, `Qty`, or `OrderId`.

**Reasoning:** The point of strong types is to make mix-ups fail to compile. With an implicit constructor, a `Price` still can't be passed as a `Qty`, but raw integers convert to any strong type, so `NewLimit{1, Side::Bid, 100, 10125}` with price and qty swapped would compile. With `explicit`, every value is labeled at the call site: `NewLimit{OrderId{1}, Side::Bid, Price{10125}, Qty{100}}`.

**Alternatives considered:**
- *Implicit constructor*: shorter tests and call sites, but swapped integer literals compile without complaint. Rejected because that's exactly the kind of bug these types exist to catch.

### Requests are a `std::variant`; the book stores `RestingOrder` (2026-10-30)

**Decision:** Incoming requests are `Request = std::variant<NewLimit, NewMarket, Cancel>`. The book stores a single concrete type, `RestingOrder`.

**Reasoning:** The set of request types is small and closed, which is what `std::variant` is for. `std::visit` doesn't compile if a case is unhandled, so adding a request type (e.g. modify) makes the compiler point out every place that needs updating. Alternatives are stored inline (size of the largest alternative plus a discriminant), so there's no per-request heap allocation or virtual dispatch. That should be fast, but it hasn't been measured yet.

Only limit orders ever rest, because a market order trades immediately and any unfilled remainder is cancelled. So the variant is only for input. If the book stored a variant, every read of a resting order would have to visit something that can only be one type.

**Alternatives considered:**
- *Inheritance* (`NewLimit`, `NewMarket` derive from a base `Request`): suits an open, extensible set of types. Here it would put every request behind a pointer, which means a heap allocation per request plus virtual dispatch, all for a set of types we know in advance.
- *One struct with an order-type enum* (like FIX's `OrdType`, with `price` ignored for market orders): simple and fixed-size, but it allows invalid states like a market order with a price, and nothing forces every type to be handled.
- *Variant for stored orders too* (`Order = std::variant<MarketOrder, LimitOrder>` in the book): rejected because market orders never rest; see above.

### Market orders use `MarketTif`, which has no GTC (2026-10-30)

**Decision:** `NewMarket::tif` is a separate `MarketTif` enum with only `ImmediateOrCancel` and `FillOrKill`. `NewLimit` keeps the full `Tif`.

**Reasoning:** A market order has no price, so its remainder can't rest on the book, and market + GTC is meaningless. Making the field's type unable to hold GTC removes that case entirely: the engine has nothing to check, assert, or reject, and matching code can't forget to handle it. It's the same reasoning as rejecting the single-struct request in favour of a variant.

The wire decoder still has to reject a GTC value in a market order message, but that's ordinary input validation it already does for any out-of-range field.

`MarketTif` and `Tif` share value names but not underlying values, so code that needs one TIF type converts with a `switch`, not a cast.

**Alternatives considered:**
- *Precondition + `TPZ_ASSERT`*: makes the engine's contract explicit, but `TPZ_ASSERT` becomes `[[assume]]` in Release, so a violation is undefined behaviour rather than a crash. Since requests will come from clients over `tpz::net`, this only works if every request source validates first, and nothing in the types enforces that.
- *Runtime reject in the engine*: safe and cheap (one predictable branch per market order), but it puts input validation into matching code and still leaves an invalid state representable.
- *Treat market + GTC as IOC*: silently changes what the client asked for. Rejected because it hides client bugs.
