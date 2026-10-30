# Phase 0 Notes: Market Fundamentals

Source: Larry Harris, *Trading and Exchanges* (2002 draft copy, Ch. 1–6 and 14). Items marked *(not in excerpt)* are filled in from general knowledge.

---

## 1. Markets and instruments (Ch. 1, §1.3)

- **Instrument**: anything traded: stocks, bonds, options, futures, FX, swaps, commodities, even betting contracts. The same microstructure ideas apply to all of them.
- **Market**: wherever traders meet to trade instruments. It can be a physical floor (old NYSE, CME) or an electronic system (Nasdaq, Euronext).
- Markets work best with **fungible** instruments, where every unit is interchangeable with every other.

## 2. Orders (Ch. 4, §4.1)

- An order is a **trade instruction** given to a broker or exchange.
- **Always specifies**: which instrument, how much, and buy or sell.
- **Optional conditions**: a price limit (most common), how long it's valid, when it can execute, whether a partial fill is OK, where to send it, and sometimes who you'll trade with.
- Harris's example: "all-or-nothing, day order to sell 7,600 XOM, limit 41.05." That means sell at no less than 41.05, only during today's session, and only if the whole quantity fills.
- Orders exist because most traders can't watch the market constantly. Traders who can react fastest to changing conditions have an advantage, which is why computerized traders win where speed matters.

## 3. Bids, offers, and the spread (Ch. 4, §4.2)

- **Bid**: an offer to buy. **Offer / ask**: an offer to sell. The quantity attached to either is its **size**.
- **Firm** prices can be demanded by others; **soft** prices can be revised before trading. Orders are firm.
- **Best bid**: the highest bid. **Best offer/ask**: the lowest ask. Together they are the **BBO** (best bid and offer). The national version is the **NBBO**.
- **Bid/ask spread** = best ask − best bid. It's also called the **inside spread**, or "the touch" in British usage.
- *(Not in excerpt)* **Mid price** = (best bid + best ask) / 2.

## 4. Liquidity: offering vs. taking (Ch. 3 & 4)

- **Liquidity** is "the ability to trade when you want to trade." Harris treats it as a *service* that is bought and sold, which is why dealers profit by selling liquidity.
- **Offering (supplying) liquidity**: posting an order that gives others a chance to trade. Both buyers and sellers can offer liquidity.
- **Standing (open) orders** offer liquidity. They stay in the book until someone trades against them, they expire, or they're canceled.
- **Taking liquidity**: accepting someone else's standing order or quote. Traders who want to trade right away **demand immediacy**.
- **Maker / taker** are the modern exchange-fee terms for offerer and taker. Harris doesn't use them in this excerpt, but they mean the same thing.
- A **liquid market** lets you trade without moving the price much. Many standing limit orders and a small spread usually mean high liquidity.

## 5. Market vs. limit orders (Ch. 2, the "Jennifer" story)

- **Market order**: buy or sell at the best price available, right now.
- **Limit order**: trade at the best price possible, but never worse than the limit price. It usually comes with an expiry; a **day order** expires at the end of the session.
- **Lifecycle of a limit order**: when it arrives, the venue first checks whether it can fill immediately. If it can't, it goes into the **limit order book** and rests there until it's matched, expires, or is canceled.
- A market order is filled against the best available price, which could come from a standing limit order in the book.

## 6. Market structure (Ch. 5)

- **Market structure** is the trading rules plus the trading systems. It decides who can trade what, when, and how, and who sees which information.
- There are two types of trading session:
  - **Continuous**: trades happen any time the market is open, in practice whenever someone demands liquidity. Almost all major markets work this way. **This is what Topaz is.**
  - **Call market**: orders are collected and all of them execute at once when the market is "called." Many continuous exchanges **open with a call auction** and also use one to restart after a trading halt.
- There are three kinds of execution system:
  - **Quote-driven**: dealers trade with customers.
  - **Order-driven**: rules match buyers to sellers. **Order precedence rules** decide who matches, and **trade pricing rules** set the trade price. **This is what Topaz is.**
  - **Brokered**: brokers find counterparties for their clients.
- The information systems are:
  - **order routing**, which sends orders in and fill reports back out
  - **order presentation**, which shows orders to traders
  - **order books**, which *store open orders*
  - **market data systems**, which report trades and quotes to the public.

## 7. Order precedence: price-time priority (Ch. 6, §6.1.1)

- **Price priority is always the primary rule.** A better price goes first: buyers can only take the lowest offer, and sellers can only hit the highest bid. You can't trade at an inferior price.
- **Secondary rules vary by market.** Futures markets use **time precedence**. US stock exchanges used **public-order precedence**, then time. Some markets also put **agency orders** (from brokers acting for clients) ahead of **proprietary orders** at the same price.
- **Time precedence rewards aggression.** If you want to jump ahead of the trader who holds time priority, you have to **improve the price by at least one tick**.
- Harris's **"leapfrog"** example: the market is 103.10 bid and 103.25 offered, and Guy holds the 103.10 bid. To get ahead of him you bid 103.15. Guy can retake priority by bidding 103.20, and so on.
- The oral-auction version described here differs from electronic FIFO. In an electronic book, *every* order at a price level waits in a queue in arrival order.

## 8. Tick size (Ch. 5 & 6)

- Prices must be an **integer multiple of the minimum price increment (tick)**. That's why the engine can store prices as integers (price in ticks).
- The tick is the cost of buying priority by improving the price. If the tick is **too small**, time priority becomes meaningless and price competition weakens. If it's **too large**, improving the price costs too much and people stop doing it. Exchanges and regulators watch tick size closely for this reason.
- **Price clustering**: traders favor round numbers. Clever traders place orders just above or below a round number to get ahead of the crowd sitting on it.
- *(Not in excerpt)* **Lot size** works the same way for quantity: the minimum quantity increment.

## 9. The spread as the price of immediacy (Ch. 14)

- Impatient traders buy at the ask and sell at the bid. **The spread is what they pay for immediacy**, and it's what limit-order traders and dealers earn for providing it.
- **Wide spread** → immediacy is expensive, so limit orders are more attractive. **Narrow spread** → market orders are cheap.
- The spread has two components:
  - **Transaction cost (transitory) component**: covers normal costs of doing business. It causes **bid/ask bounce**, where trade prices flip between bid and ask as buyers and sellers alternate.
  - **Adverse selection component**: covers losses to better-informed traders, who pick which side to trade on.
- Harris's "most important lesson": **uninformed traders lose to informed traders, whether they use limit or market orders.**

---

## Not in the excerpt (filled in)

- **Crossing the spread**: a buy priced ≥ best ask, or a sell priced ≤ best bid, is *marketable* and trades immediately. The resting order's price is the trade price.
- **Fill vs. partial fill**: a fill executes the whole quantity; a partial fill executes only some of it. An incoming limit order matches as far as its limit price allows, and **any leftover rests in the book at its limit price**. A leftover market order is usually canceled.
- **All-or-nothing (AON) / fill-or-kill (FOK)**: a condition on *quantity*, not price. The whole quantity must execute, but it can do so across multiple price levels and counterparties, as long as every execution is within the limit. FOK cancels immediately if it can't fully fill; resting AON is messy and many venues don't support it.
- **Market data levels**:
  - **L1**: the BBO and sizes.
  - **L2**: the total quantity at each price level.
  - **L3**: every individual order, with its ID.

## What this means for Topaz

- An order is roughly {instrument, side, qty, limit price, time-in-force}.
- Matching: an incoming order sweeps the opposite side best price first and FIFO within each level, stops at its limit, and the remainder rests.
- Prices are integer ticks.
- AON/FOK needs a check pass first: sum available quantity up to the limit before matching anything.
- An opening call auction is a natural "later feature."