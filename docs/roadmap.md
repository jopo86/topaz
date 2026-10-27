# Topaz Roadmap

A checklist to work through roughly in order. Each phase has **Learn** items (concepts to understand first) and **Build** items (code to write). Don't optimize anything until Phase 5. Getting a correct, well-tested slow version first is the whole point of Phases 1–4.

Check items off as you go (`- [x]`). Add notes inline when something surprised you.

---

## Phase 0: Market fundamentals (no code)

Goal: be able to explain what an exchange does and what an order book is to someone else.

**Learn**
- [ ] What an exchange is: a venue that matches buyers and sellers of an *instrument* (a stock, future, etc.)
- [ ] **Bid** (price someone will buy at) vs **ask/offer** (price someone will sell at)
- [ ] **Best bid / best ask** ("top of book"), the **spread** (ask − bid), and the **mid price**
- [ ] **Limit order**: buy/sell up to N units at price P or better. It rests in the book if it can't trade right away.
- [ ] **Market order**: trade N units right now at whatever prices are available
- [ ] **Crossing the spread**: a buy priced ≥ best ask (or a sell ≤ best bid) trades immediately
- [ ] **Maker** (adds resting liquidity) vs **taker** (removes it by trading against resting orders)
- [ ] **Fill** vs **partial fill**; what happens to the leftover quantity
- [ ] **Price-time priority (FIFO)**: better price matches first, and at equal prices the earlier order matches first
- [ ] **Tick size** (minimum price increment) and **lot size** (minimum quantity increment). This is why prices are integers here.
- [ ] Market data levels: **L1** (top of book), **L2** (total quantity per price level), **L3** (every individual order)
- [ ] Read through a few real order book screenshots/visualizations and narrate what happens when a new order arrives

**Check yourself**
- [ ] On paper: given a book and an incoming order, write down every trade it produces and what the book looks like afterward. Do this for a buy that partially fills across 3 price levels.

**Resources**
- Larry Harris, *Trading and Exchanges* (chapters on orders and order books; the rest is optional)
- Investopedia entries for any term above you're unsure of
- WK Selph, "How to Build a Fast Limit Order Book" (short, classic blog post; skim now, reread in Phase 5)

---

## Phase 1: Core types and a naive, correct book

Goal: a simple order book using standard containers that matches limit orders correctly.

**Learn**
- [ ] Fixed-point prices: storing `$101.25` with tick `0.01` as the integer `10125`, and why floats are wrong for money
- [ ] Why the engine *emits events* instead of returning results or printing (see CLAUDE.md: separation of concerns)
- [ ] Determinism: why the same inputs must always produce the same outputs (replay, debugging, auditing)

**Build**
- [ ] Decide where shared types (`Price`, `Qty`, `OrderId`, `Side`) live, and resolve the "Confirm" note in CLAUDE.md
- [ ] Strong types for price/qty/id (so you can't pass a `Qty` where a `Price` is expected)
- [ ] `Order` struct: id, side, price, quantity remaining
- [ ] Naive book: e.g. `std::map<Price, std::list<Order>>` per side (think about which way each side should sort)
- [ ] `add_limit`: match against the opposite side while it crosses, then rest the remainder
- [ ] `cancel(id)`: needs a way to find an order by id (what's the complexity of your first attempt?)
- [ ] Output events: `Accepted`, `Trade`/`Fill`, `Cancelled`, `Rejected` (with a reason)
- [ ] Reject invalid input (zero qty, unknown id on cancel, duplicate id) using `std::expected`
- [ ] Delete the `placeholder` files

**Tests** (`tests/engine/`): one test per rule, written before or alongside the code
- [ ] Non-crossing order rests at the correct level
- [ ] Crossing order fills fully against one resting order
- [ ] Partial fill: incoming larger than resting, and resting larger than incoming
- [ ] Sweep across multiple price levels, best price first
- [ ] FIFO at the same price level
- [ ] Trade price is the *resting* order's price, not the incoming one's
- [ ] Cancel removes the order; cancelling a filled/unknown order is rejected
- [ ] Empty levels are removed; best bid/ask update correctly

**DSA used**: balanced BST (`std::map`), linked list, hash map (id → order)

---

## Phase 2: Test infrastructure

Goal: confidence that later optimizations don't break anything.

**Learn**
- [ ] **Differential testing**: run two implementations on the same input and assert identical output
- [ ] **Golden/snapshot tests**: a recorded input sequence and its expected event output

**Build**
- [ ] A simple text format for scripted scenarios (e.g. `BUY 100 @ 10125 id=1`) plus a parser in `tests/common/`
- [ ] Scenario-driven tests that compare emitted events against expected events
- [ ] A random order-flow generator with a fixed seed (reproducible)
- [ ] Book invariant checker: best bid < best ask, no empty levels, quantities positive, per-level totals add up
- [ ] Keep the Phase 1 naive book around as the **reference implementation** for differential tests later

---

## Phase 3: More order types and operations

**Learn**
- [ ] **Market order**: what to do when there isn't enough liquidity (cancel the remainder vs. reject)
- [ ] **Time in force**: GTC (good till cancel), **IOC** (immediate or cancel: fill what you can, cancel the rest), **FOK** (fill or kill: all or nothing)
- [ ] **Post-only**: reject (or reprice) if it would trade immediately; used by makers
- [ ] **Modify / cancel-replace**: when does an order *lose* its time priority? (Commonly: a price change or quantity increase loses it, a quantity decrease keeps it.)
- [ ] *(Optional)* **Self-trade prevention**: don't let a trader match against their own order

**Build + test each**
- [ ] Market orders
- [ ] IOC
- [ ] FOK (hint: you need to check available liquidity *before* mutating the book)
- [ ] Post-only
- [ ] Modify with correct priority rules
- [ ] Record your priority-rule choices in `docs/decisions.md`

---

## Phase 4: Measure the baseline

Goal: numbers to beat before you change any data structures.

**Learn**
- [ ] Why **latency percentiles** (p50, p99, p99.9) matter more than the average for a matching engine
- [ ] How Google Benchmark works: `DoNotOptimize`, `ClobberMemory`, fixtures, `Range`
- [ ] Why benchmarks need realistic workloads: most real orders are placed near the top of book, and most get cancelled
- [ ] Basic profiling on your platform (Visual Studio profiler, `perf` on Linux, or Tracy)

**Build** (`bench/engine/`)
- [ ] Workload generator: configurable mix of add/cancel/match, prices clustered near mid
- [ ] `bm_add_non_crossing`, `bm_cancel`, `bm_match_sweep`, `bm_mixed_workload`
- [ ] Per-operation latency histogram (custom harness or Benchmark counters)
- [ ] Record baseline numbers + machine/compiler in `docs/`

---

## Phase 5: Performance-oriented data structures (`tpz::dsa`)

Goal: replace standard containers one at a time, measuring each change. Run the differential tests after every swap.

**Learn**
- [ ] CPU caches, cache lines (64 bytes), and why pointer-chasing (`std::map`, `std::list`) is slow
- [ ] Cost of heap allocation on the hot path; object pools and free lists
- [ ] **Intrusive** data structures: the links live inside the element, so there's no separate node allocation
- [ ] Open-addressing hash tables (linear probing) vs. chaining (`std::unordered_map`)
- [ ] Struct layout: field order, padding, `alignas`, hot/cold splitting
- [ ] Carl Cook, "When a Microsecond Is an Eternity" (CppCon 2017 talk)

**Build** (one at a time: benchmark, test, record each in `docs/decisions.md`)
- [ ] Order pool: preallocated array of `Order` slots + free list (no `new` per order)
- [ ] Intrusive doubly linked list for orders within a price level (O(1) cancel given a pointer)
- [ ] Order id → order lookup: open-addressing hash map, or a direct array if ids are dense
- [ ] Price level storage. Compare at least two:
  - [ ] Sorted `std::vector` of levels (best price at the back)
  - [ ] Array indexed by `price - base_price` (O(1) lookup, bounded price range)
  - [ ] Your Phase 1 `std::map` (baseline)
- [ ] Track best bid/ask efficiently when a level empties
- [ ] Unit tests for every `tpz::dsa` structure in `tests/dsa/`, with benchmarks in `bench/dsa/`
- [ ] Re-run Phase 4 benchmarks and compare against the baseline

**DSA used**: arrays, free lists, intrusive linked lists, open addressing, amortized analysis

---

## Phase 6: Engine API

**Learn**
- [ ] Input commands vs. output events as the engine's only interface
- [ ] **Sequence numbers**: every input and output gets one, so consumers can detect gaps and replay
- [ ] Multiple instruments: one book per symbol, routed by an instrument id

**Build**
- [ ] `Engine` that owns books and routes commands by instrument
- [ ] An event sink interface that avoids virtual dispatch on the hot path (template parameter, or a ring buffer the consumer drains)
- [ ] Replay: feed a recorded command log and get byte-identical output events

---

## Phase 7: Networking (`tpz::net`)

**Learn**
- [ ] TCP vs. UDP, and why order entry usually uses TCP while market data uses UDP multicast
- [ ] Binary wire protocols: fixed-size messages, explicit endianness (`std::byteswap`), message framing
- [ ] Skim the Nasdaq **ITCH** (market data) and **OUCH** (order entry) specs for real-world design ideas
- [ ] Threading model: network thread ↔ engine thread via a **single-producer single-consumer (SPSC) queue**

**Build**
- [ ] Bring in the Garnet-derived socket layer
- [ ] Order entry protocol: encode/decode + tests (round-trip, malformed input)
- [ ] Market data feed: publish book updates and trades
- [ ] SPSC ring buffer in `tpz::dsa` (with tests + benchmark)
- [ ] A small client that sends orders and prints executions

---

## Phase 8: GUI example (`examples/gui/`)

- [ ] Dear ImGui app skeleton + `examples/gui/CMakeLists.txt`
- [ ] Ladder view: bids and asks by price level with quantities
- [ ] Order entry panel (side, price, qty, type)
- [ ] Trade tape (recent fills)
- [ ] *(Optional)* Depth chart; connect over the network instead of linking the engine directly

---

## Stretch goals

- [ ] Replay real historical data (public ITCH sample files) and benchmark against it
- [ ] Opening/closing **auctions** (uncross at a single price that maximizes volume)
- [ ] Stop orders, iceberg (hidden quantity) orders
- [ ] Fuzzing the protocol decoder and the book (libFuzzer)
- [ ] Write up results in `README.md`: design, benchmark numbers, what you learned
