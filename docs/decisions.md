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
