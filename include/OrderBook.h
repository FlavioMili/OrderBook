#ifndef ORDERBOOK_INCLUDED
#define ORDERBOOK_INCLUDED

#include "Order.h"
#include "OrderPool.h"
#include <limits>
#include <map>
#include <utility>
#include <deque>
#include <iostream>
#include <memory>

class TestOrderBook;

struct BestPrice {
    double price;
    Order* order;
};

class OrderBook {
private:
  OrderPool orderPool;
  std::map<double, std::deque<Order*>, std::greater<double>> BuyOrders;
  std::map<double, std::deque<Order*>> SellOrders;
  BestPrice bidMax {};
  BestPrice askMin {std::numeric_limits<double>::infinity(), nullptr};

  void updateBidMax();
  void updateAskMin();
  void processBuyMatching(double price, int& quantity);
  void processSellMatching(double price, int& quantity);

  template<typename MapType>
  void removeFrontOrder(MapType& ordersMap, double price) {
    auto& ordersAtPrice = ordersMap[price];
    orderPool.deallocate(ordersAtPrice.front());
    ordersAtPrice.pop_front();

    if (ordersAtPrice.empty()) {
      ordersMap.erase(price);
    }
  }
  template<typename MapType>
  void printOrders(const MapType& ordersMap) const {
    for (const auto& [price, orders] : ordersMap) {
      for (const auto& order : orders) {
        std::cout << "ID: " << order->ID 
          << ", Price: " << order->price 
          << ", Quantity: " << order->quantity << "\n";
      }
    }
  }

public:
  OrderBook(size_t poolSize);

  template<typename MapType>
  void static printHistogram(const MapType& ordersMap, int blockSize, const std::string& color) {
    for (const auto& [price, orders] : ordersMap) {
      int totalQuantity = 0;
      for (const auto& order : orders) {
        totalQuantity += order->quantity;
      }
      int symbolCount = totalQuantity / blockSize;

      std::cout << price << ": " << color;
      for (int i = 0; i < symbolCount; ++i) {
        std::cout << "\u2587";
      }
      std::cout << "\033[0m\n"; // Reset color
    }
  }

  void processOrders(OrderSide side, double price, int quantity, time_t timestamp, int ID);
  void printOrderBook();

  // TODO later on
  // bool cancelOrder(int orderID);
  // bool modifyOrder(int orderID, double newPrice, int newQuantity);

  const std::map<double, std::deque<Order*>, std::greater<double>>& getBuyOrders() const {
    return BuyOrders;
  }

  const std::map<double, std::deque<Order*>>& getSellOrders() const {
    return SellOrders;
  }
  friend class TestOrderBook;
};

#endif // !ORDERBOOK_INCLUDED
