#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <algorithm>
#include <iomanip>

class Order {
public:
    std::string order_id;
    std::string instrument;
    std::string side;
    double price;
    int quantity;
    double timestamp;

    Order(const std::string& id, const std::string& inst, const std::string& s, double p, int q, double ts = 0.0)
        : order_id(id), instrument(inst), side(s), price(p), quantity(q) {
        timestamp = (ts == 0.0) ? std::chrono::duration_cast<std::chrono::duration<double>>(
            std::chrono::system_clock::now().time_since_epoch()).count() : ts;
        std::transform(instrument.begin(), instrument.end(), instrument.begin(), ::toupper);
        std::transform(side.begin(), side.end(), side.begin(), ::toupper);
    }

    void print() const {
        std::cout << "Order(id=" << order_id << ", inst=" << instrument
                  << ", side=" << side << ", price=" << std::fixed << std::setprecision(2) << price
                  << ", qty=" << quantity << ", time=" << timestamp << ")" << std::endl;
    }
};

class Trade {
public:
    std::string instrument;
    std::string buy_order_id;
    std::string sell_order_id;
    double price;
    int quantity;

    Trade(const std::string& inst, const std::string& buy_id, const std::string& sell_id, double p, int q)
        : instrument(inst), buy_order_id(buy_id), sell_order_id(sell_id), price(p), quantity(q) {}

    void print() const {
        std::cout << "Trade(inst=" << instrument << ", buy_id=" << buy_order_id
                  << ", sell_id=" << sell_order_id << ", price=" << std::fixed << std::setprecision(2) << price
                  << ", qty=" << quantity << ")" << std::endl;
    }
};

class OrderBook {
private:
    struct InstrumentBook {
        std::map<double, std::vector<Order>, std::greater<double>> buy_orders; // Sorted by descending price
        std::map<double, std::vector<Order>> sell_orders; // Sorted by ascending price
        std::vector<Trade> trades;
    };

    std::map<std::string, InstrumentBook> order_books;

    void add_buy_order(Order& order) {
        std::string instrument = order.instrument;
        auto& buy_orders = order_books[instrument].buy_orders;
        auto& sell_orders = order_books[instrument].sell_orders;
        auto& trades = order_books[instrument].trades;

        while (order.quantity > 0 && !sell_orders.empty()) {
            auto it = sell_orders.begin();
            double best_sell_price = it->first;
            if (best_sell_price > order.price) break;
            auto& sell_order_list = it->second;
            if (sell_order_list.empty()) {
                sell_orders.erase(it);
                continue;
            }
            Order& sell_order = sell_order_list[0];
            int trade_quantity = std::min(order.quantity, sell_order.quantity);
            double trade_price = sell_order.price;
            trades.emplace_back(instrument, order.order_id, sell_order.order_id, trade_price, trade_quantity);
            order.quantity -= trade_quantity;
            sell_order.quantity -= trade_quantity;
            if (sell_order.quantity == 0) {
                sell_order_list.erase(sell_order_list.begin());
                if (sell_order_list.empty()) {
                    sell_orders.erase(it);
                }
            }
        }
        if (order.quantity > 0) {
            buy_orders[order.price].push_back(order);
            std::sort(buy_orders[order.price].begin(), buy_orders[order.price].end(),
                      [](const Order& a, const Order& b) { return a.timestamp < b.timestamp; });
        }
    }

    void add_sell_order(Order& order) {
        std::string instrument = order.instrument;
        auto& buy_orders = order_books[instrument].buy_orders;
        auto& sell_orders = order_books[instrument].sell_orders;
        auto& trades = order_books[instrument].trades;

        while (order.quantity > 0 && !buy_orders.empty()) {
            auto it = buy_orders.begin();
            double best_buy_price = it->first;
            if (best_buy_price < order.price) break;
            auto& buy_order_list = it->second;
            if (buy_order_list.empty()) {
                buy_orders.erase(it);
                continue;
            }
            Order& buy_order = buy_order_list[0];
            int trade_quantity = std::min(order.quantity, buy_order.quantity);
            double trade_price = buy_order.price;
            trades.emplace_back(instrument, buy_order.order_id, order.order_id, trade_price, trade_quantity);
            order.quantity -= trade_quantity;
            buy_order.quantity -= trade_quantity;
            if (buy_order.quantity == 0) {
                buy_order_list.erase(buy_order_list.begin());
                if (buy_order_list.empty()) {
                    buy_orders.erase(it);
                }
            }
        }
        if (order.quantity > 0) {
            sell_orders[order.price].push_back(order);
            std::sort(sell_orders[order.price].begin(), sell_orders[order.price].end(),
                      [](const Order& a, const Order& b) { return a.timestamp < b.timestamp; });
        }
    }

public:
    std::vector<Trade> add_order(Order& order) {
        order_books[order.instrument].trades.clear();
        if (order.side == "BUY") {
            add_buy_order(order);
        } else if (order.side == "SELL") {
            add_sell_order(order);
        }
        return order_books[order.instrument].trades;
    }

    void print_order_book_state(const std::string& instrument) {
        const auto& book = order_books[instrument];
        std::cout << "Instrument: " << instrument << "\nBuy Orders:\n";
        for (const auto& [price, orders] : book.buy_orders) {
            std::cout << "  Price: " << std::fixed << std::setprecision(2) << price
                      << ", Orders: " << orders.size() << "\n";
        }
        std::cout << "Sell Orders:\n";
        for (const auto& [price, orders] : book.sell_orders) {
            std::cout << "  Price: " << std::fixed << std::setprecision(2) << price
                      << ", Orders: " << orders.size() << "\n";
        }
        std::cout << "Trades:\n";
        for (const auto& trade : book.trades) {
            trade.print();
        }
    }
};

std::vector<Order> generate_orders_from_data(const std::string& instrument,
                                            const std::map<std::string, double>& price_data,
                                            int num_orders = 10) {
    std::vector<Order> orders;
    std::vector<double> prices;
    for (const auto& [_, price] : price_data) {
        prices.push_back(price);
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> price_spread(-0.5, 0.5);
    std::uniform_int_distribution<> quantity_dist(1, 100);
    std::vector<std::string> sides = {"BUY", "SELL"};

    for (int i = 1; i <= num_orders; ++i) {
        std::uniform_int_distribution<> price_idx(0, prices.size() - 1);
        double price = prices[price_idx(gen)] + price_spread(gen);
        price = std::round(price * 100.0) / 100.0; // Round to 2 decimal places
        std::string side = sides[std::uniform_int_distribution<>(0, 1)(gen)];
        int quantity = quantity_dist(gen);
        orders.emplace_back(instrument + "_" + std::to_string(i), instrument, side, price, quantity);
    }
    return orders;
}

int main() {
    // Sample 1-day price data (from provided financial data, May 29, 2025)
    std::map<std::string, double> spy_prices = {
        {"09:30", 591.03}, {"09:45", 591.2308}, {"10:00", 589.7}, {"10:15", 590.09},
        {"10:30", 589.735}, {"10:45", 590.93}, {"11:00", 589.61}, {"11:15", 590.72},
        {"11:30", 589.19}, {"11:45", 587.19}, {"12:00", 588.58}, {"12:15", 588.99},
        {"12:30", 589.515}, {"12:45", 589.795}, {"13:00", 589.81}, {"13:15", 589.32},
        {"13:30", 589.21}, {"13:45", 588.5551}, {"14:00", 589.09}, {"14:15", 588.03},
        {"14:30", 589.215}, {"14:45", 588.7901}, {"15:00", 588.525}, {"15:15", 588.89},
        {"15:30", 589.28}, {"15:45", 590.0}, {"16:00", 589.45}, {"16:15", 589.91},
        {"16:30", 589.66}, {"16:45", 589.4}, {"17:00", 589.5098}, {"17:15", 589.45},
        {"17:30", 589.31}, {"17:45", 589.0012}, {"18:00", 589.64}, {"18:15", 588.66},
        {"18:30", 588.45}, {"18:45", 588.7784}, {"19:00", 588.9299}, {"19:15", 588.95},
        {"19:30", 588.42}, {"19:45", 588.1}
    };
    std::map<std::string, double> msft_prices = {
        {"09:30", 459.51}, {"09:45", 458.745}, {"10:00", 458.0}, {"10:15", 458.77},
        {"10:30", 459.23}, {"10:45", 459.725}, {"11:00", 458.6}, {"11:15", 459.27},
        {"11:30", 458.73}, {"11:45", 456.05}, {"12:00", 458.2}, {"12:15", 458.55},
        {"12:30", 458.96}, {"12:45", 459.1085}, {"13:00", 458.83}, {"13:15", 458.5372},
        {"13:30", 458.415}, {"13:45", 458.08}, {"14:00", 458.16}, {"14:15", 457.51},
        {"14:30", 458.2941}, {"14:45", 457.86}, {"15:00", 457.68}, {"15:15", 458.075},
        {"15:30", 458.3}, {"15:45", 458.45}, {"16:00", 457.7}, {"16:15", 458.12},
        {"16:30", 458.66}, {"16:45", 458.44}, {"17:00", 458.0514}, {"17:15", 458.25},
        {"17:30", 458.47}, {"17:45", 458.47}, {"18:00", 458.25}, {"18:15", 457.6701},
        {"19:00", 457.946}, {"19:15", 457.98}, {"19:45", 457.0}
    };

    OrderBook order_book;

    // Generate orders for SPY and MSFT
    auto spy_orders = generate_orders_from_data("SPY", spy_prices, 10);
    auto msft_orders = generate_orders_from_data("MSFT", msft_prices, 10);

    // Process SPY orders
    std::cout << "Processing SPY Orders:\n";
    for (auto& order : spy_orders) {
        std::cout << "\nAdding order: ";
        order.print();
        auto trades = order_book.add_order(order);
        std::cout << "Trades executed:\n";
        for (const auto& trade : trades) {
            trade.print();
        }
        std::cout << "SPY Order book state:\n";
        order_book.print_order_book_state("SPY");
    }

    // Process MSFT orders
    std::cout << "\nProcessing MSFT Orders:\n";
    for (auto& order : msft_orders) {
        std::cout << "\nAdding order: ";
        order.print();
        auto trades = order_book.add_order(order);
        std::cout << "Trades executed:\n";
        for (const auto& trade : trades) {
            trade.print();
        }
        std::cout << "MSFT Order book state:\n";
        order_book.print_order_book_state("MSFT");
    }

    return 0;
}