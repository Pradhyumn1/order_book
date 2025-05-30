
# Order Book/Matching Engine for Algorithmic Trading

## Overview
This project implements a matching engine for algorithmic trading, supporting buy and sell orders for multiple instruments (SPY and MSFT). Written in C++, it simulates an order book that matches orders based on price-time priority and generates realistic orders using provided 1-day price data from May 29, 2025. The engine processes orders, executes trades, and displays the order book state.

## Features
- **Order Management**: Handles buy and sell limit orders for SPY (S&P 500 ETF) and MSFT (Microsoft) with attributes: order ID, instrument, side, price, quantity, and timestamp.
- **Order Matching**: Matches orders based on price (best available) and time priority (earliest first).
- **Trade Execution**: Generates trades when buy and sell orders match, tracking instrument, order IDs, price, and quantity.
- **Price Data**: Uses historical 1-day price data for SPY (~$588.10–$591.03) and MSFT (~$457.00–$459.51) to generate realistic orders.
- **Console Output**: Displays orders, executed trades, and order book state for each instrument.

## Prerequisites
- C++11 or later compatible compiler (e.g., g++ or clang++).
- No external libraries are required (uses C++ Standard Library only).
- Operating system: Linux, macOS, or Windows with a C++ build environment.

## Project Structure
```
stocknprediction/
└── matching_engine.cpp
```

## Setup Instructions
1. **Clone or Download**:
   - Save `matching_engine.cpp` to your project directory (e.g., `/Users/pradhyumnyadav/Desktop/stocknprediction/`).

2. **Compile**:
   - Navigate to the project directory:
     ```
     cd /Users/pradhyumnyadav/Desktop/stocknprediction
     ```
   - Compile using a C++11 or later compiler:
     ```
     g++ -std=c++11 matching_engine.cpp -o matching_engine
     ```

3. **Run**:
   - Execute the compiled program:
     ```
     ./matching_engine
     ```

## Usage
- The program simulates trading by generating 10 random orders each for SPY and MSFT, with prices based on the provided 1-day data (May 29, 2025) and a ±0.5 spread.
- Orders are processed sequentially, with the following output for each:
  - Order details (ID, instrument, side, price, quantity, timestamp).
  - Executed trades (if any).
  - Current order book state (buy/sell orders by price level and trade history).
- Example output:
  ```
  Processing SPY Orders:
  Adding order: Order(id=SPY_1, inst=SPY, side=BUY, price=589.23, qty=50, time=...)
  Trades executed:
  SPY Order book state:
  Instrument: SPY
  Buy Orders:
    Price: 589.23, Orders: 1
  Sell Orders:
  Trades:

  ...

  Processing MSFT Orders:
  Adding order: Order(id=MSFT_1, inst=MSFT, side=BUY, price=458.50, qty=20, time=...)
  Trades executed:
  MSFT Order book state:
  Instrument: MSFT
  Buy Orders:
    Price: 458.50, Orders: 1
  Sell Orders:
  Trades:
  ```

## Notes
- **Data**: The engine uses hardcoded SPY and MSFT price data from May 29, 2025. Modify the `spy_prices` and `msft_prices` maps in `matching_engine.cpp` to use different data.
- **Extensibility**: To integrate with a web UI (e.g., HTML/CSS/JS with Flask), a C++ web framework like Crow or Drogon is needed. Alternatively, you can interface with the existing Flask backend by rewriting the API to call this C++ engine.
- **Performance**: The implementation uses `std::map` and `std::vector` for efficient order management. For high-frequency trading, consider optimizing with custom allocators or priority queues.
- **Customization**: To add features like order cancellation or market orders, modify the `OrderBook` class. Share specific requirements for further enhancements.
- **Favicon (Web Context)**: If integrating with a web UI, handle `/favicon.ico` requests to avoid 404 errors by adding a favicon file and route (relevant for the Flask version).

## Troubleshooting
- **Compilation Errors**: Ensure a C++11 or later compiler is used. Check for syntax errors or missing headers.
- **Output Formatting**: Adjust the `print` methods in `Order` and `Trade` classes for different output formats (e.g., JSON).
- **Randomness**: Random order generation uses `std::random_device`. For deterministic testing, set a fixed seed in the `std::mt19937` generator.

## Future Enhancements
- Add support for order cancellation or modification.
- Integrate with a real-time price feed for dynamic order generation.
- Extend to a web-based UI using a C++ web framework or by interfacing with the existing Flask API.
- Optimize for large-scale order books with advanced data structures.

## Contact
For issues or feature requests, please provide details about your requirements or any errors encountered.
