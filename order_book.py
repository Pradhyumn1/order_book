# Matching Engine for Algorithmic Trading with SPY and MSFT Data
from collections import defaultdict
from typing import List, Tuple
import time
import random

class Order:
    """Represents a single order (buy or sell) for a specific instrument."""
    def __init__(self, order_id: str, instrument: str, side: str, price: float, quantity: int, timestamp: float = None):
        self.order_id = order_id
        self.instrument = instrument.upper()  # e.g., 'SPY' or 'MSFT'
        self.side = side.upper()  # 'BUY' or 'SELL'
        self.price = price
        self.quantity = quantity
        self.timestamp = timestamp if timestamp else time.time()

    def __repr__(self):
        return f"Order(id={self.order_id}, inst={self.instrument}, side={self.side}, price={self.price}, qty={self.quantity}, time={self.timestamp})"

class Trade:
    """Represents a matched trade between a buy and sell order."""
    def __init__(self, instrument: str, buy_order_id: str, sell_order_id: str, price: float, quantity: int):
        self.instrument = instrument
        self.buy_order_id = buy_order_id
        self.sell_order_id = sell_order_id
        self.price = price
        self.quantity = quantity

    def __repr__(self):
        return f"Trade(inst={self.instrument}, buy_id={self.buy_order_id}, sell_id={self.sell_order_id}, price={self.price}, qty={self.quantity})"

class OrderBook:
    """Manages buy and sell orders for multiple instruments and matches them."""
    def __init__(self):
        # Dictionary of order books per instrument: instrument -> {buy_orders, sell_orders}
        self.order_books = defaultdict(lambda: {'buy_orders': defaultdict(list), 'sell_orders': defaultdict(list), 'trades': []})

    def add_order(self, order: Order) -> List[Trade]:
        """Add a new order to the order book and attempt to match it."""
        instrument = order.instrument
        self.order_books[instrument]['trades'] = []  # Reset trades for this instrument
        if order.side == 'BUY':
            self._add_buy_order(order)
        elif order.side == 'SELL':
            self._add_sell_order(order)
        return self.order_books[instrument]['trades']

    def _add_buy_order(self, order: Order):
        """Add a buy order and attempt to match with sell orders."""
        instrument = order.instrument
        buy_orders = self.order_books[instrument]['buy_orders']
        sell_orders = self.order_books[instrument]['sell_orders']
        trades = self.order_books[instrument]['trades']
        
        while order.quantity > 0 and sell_orders:
            best_sell_price = min(sell_orders.keys(), default=None)
            if best_sell_price is None or best_sell_price > order.price:
                break
            sell_order = sell_orders[best_sell_price][0]
            trade_quantity = min(order.quantity, sell_order.quantity)
            trade_price = sell_order.price
            trades.append(Trade(instrument, order.order_id, sell_order.order_id, trade_price, trade_quantity))
            order.quantity -= trade_quantity
            sell_order.quantity -= trade_quantity
            if sell_order.quantity == 0:
                sell_orders[best_sell_price].pop(0)
                if not sell_orders[best_sell_price]:
                    del sell_orders[best_sell_price]
        if order.quantity > 0:
            buy_orders[order.price].append(order)
            buy_orders[order.price].sort(key=lambda x: x.timestamp)

    def _add_sell_order(self, order: Order):
        """Add a sell order and attempt to match with buy orders."""
        instrument = order.instrument
        buy_orders = self.order_books[instrument]['buy_orders']
        sell_orders = self.order_books[instrument]['sell_orders']
        trades = self.order_books[instrument]['trades']
        
        while order.quantity > 0 and buy_orders:
            best_buy_price = max(buy_orders.keys(), default=None)
            if best_buy_price is None or best_buy_price < order.price:
                break
            buy_order = buy_orders[best_buy_price][0]
            trade_quantity = min(order.quantity, buy_order.quantity)
            trade_price = buy_order.price
            trades.append(Trade(instrument, buy_order.order_id, order.order_id, trade_price, trade_quantity))
            order.quantity -= trade_quantity
            buy_order.quantity -= trade_quantity
            if buy_order.quantity == 0:
                buy_orders[best_buy_price].pop(0)
                if not buy_orders[best_buy_price]:
                    del buy_orders[best_buy_price]
        if order.quantity > 0:
            sell_orders[order.price].append(order)
            sell_orders[order.price].sort(key=lambda x: x.timestamp)

    def get_order_book_state(self, instrument: str) -> dict:
        """Return the current state of the order book for a specific instrument."""
        return {
            'instrument': instrument,
            'buy_orders': {price: len(orders) for price, orders in sorted(self.order_books[instrument]['buy_orders'].items(), reverse=True)},
            'sell_orders': {price: len(orders) for price, orders in sorted(self.order_books[instrument]['sell_orders'].items())},
            'trades': self.order_books[instrument]['trades']
        }

def generate_orders_from_data(instrument: str, price_data: dict, num_orders: int = 10) -> List[Order]:
    """Generate realistic orders based on price data."""
    orders = []
    prices = list(price_data.values())
    for i in range(num_orders):
        price = random.choice(prices) + random.uniform(-0.5, 0.5)  # Add small spread
        price = round(price, 2)
        side = random.choice(['BUY', 'SELL'])
        quantity = random.randint(1, 100)
        order = Order(f"{instrument}_{i+1}", instrument, side, price, quantity)
        orders.append(order)
    return orders

def main():
    """Simulate trading with SPY and MSFT data."""
    # Sample 1-day price data (from provided financial data, May 29, 2025)
    spy_prices = {
        "09:30": 591.03, "09:45": 591.2308, "10:00": 589.7, "10:15": 590.09,
        "10:30": 589.735, "10:45": 590.93, "11:00": 589.61, "11:15": 590.72,
        "11:30": 589.19, "11:45": 587.19, "12:00": 588.58, "12:15": 588.99,
        "12:30": 589.515, "12:45": 589.795, "13:00": 589.81, "13:15": 589.32,
        "13:30": 589.21, "13:45": 588.5551, "14:00": 589.09, "14:15": 588.03,
        "14:30": 589.215, "14:45": 588.7901, "15:00": 588.525, "15:15": 588.89,
        "15:30": 589.28, "15:45": 590.0, "16:00": 589.45, "16:15": 589.91,
        "16:30": 589.66, "16:45": 589.4, "17:00": 589.5098, "17:15": 589.45,
        "17:30": 589.31, "17:45": 589.0012, "18:00": 589.64, "18:15": 588.66,
        "18:30": 588.45, "18:45": 588.7784, "19:00": 588.9299, "19:15": 588.95,
        "19:30": 588.42, "19:45": 588.1
    }
    msft_prices = {
        "09:30": 459.51, "09:45": 458.745, "10:00": 458.0, "10:15": 458.77,
        "10:30": 459.23, "10:45": 459.725, "11:00": 458.6, "11:15": 459.27,
        "11:30": 458.73, "11:45": 456.05, "12:00": 458.2, "12:15": 458.55,
        "12:30": 458.96, "12:45": 459.1085, "13:00": 458.83, "13:15": 458.5372,
        "13:30": 458.415, "13:45": 458.08, "14:00": 458.16, "14:15": 457.51,
        "14:30": 458.2941, "14:45": 457.86, "15:00": 457.68, "15:15": 458.075,
        "15:30": 458.3, "15:45": 458.45, "16:00": 457.7, "16:15": 458.12,
        "16:30": 458.66, "16:45": 458.44, "17:00": 458.0514, "17:15": 458.25,
        "17:30": 458.47, "17:45": 458.47, "18:00": 458.25, "18:15": 457.6701,
        "19:00": 457.946, "19:15": 457.98, "19:45": 457.0
    }

    order_book = OrderBook()

    # Generate orders for SPY and MSFT
    spy_orders = generate_orders_from_data('SPY', spy_prices, num_orders=10)
    msft_orders = generate_orders_from_data('MSFT', msft_prices, num_orders=10)

    # Process orders
    print("Processing SPY Orders:")
    for order in spy_orders:
        print(f"\nAdding order: {order}")
        trades = order_book.add_order(order)
        print("Trades executed:", trades)
        print("SPY Order book state:", order_book.get_order_book_state('SPY'))

    print("\nProcessing MSFT Orders:")
    for order in msft_orders:
        print(f"\nAdding order: {order}")
        trades = order_book.add_order(order)
        print("Trades executed:", trades)
        print("MSFT Order book state:", order_book.get_order_book_state('MSFT'))

if __name__ == "__main__":
    main()