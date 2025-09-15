#ifndef ORDERBOOK_INCLUDED
#define ORDERBOOK_INCLUDED

#include "Order.h"
#include "OrderPool.h"
#include <vector>
#include <deque>
#include <cmath>

class TestOrderBook;

class OrderBook {
private:
  OrderPool orderPool;
  std::vector<std::deque<Order*>> BuyOrders;
  std::vector<std::deque<Order*>> SellOrders;

  static constexpr double minPrice = 50.0;
  static constexpr double tickSize = 0.1;
  static constexpr size_t priceLevels = 501;

  int bestBidIndex = -1;
  int bestAskIndex = -1;

  size_t priceToIndex(double price) const {
    return static_cast<size_t>(std::round((price - minPrice) / tickSize));
  }

  double indexToPrice(size_t index) const {
    return minPrice + (index * tickSize);
  }

  void updateBestBid();
  void updateBestAsk();
  void processBuyMatching(uint32_t& quantity, size_t index);
  void processSellMatching(uint32_t& quantity, size_t index);
  void removeFrontOrder(bool isBuySide, size_t index);

public:
  OrderBook();
  void processOrders(bool isBuy, double price, uint32_t quantity, uint32_t timestamp, uint32_t ID);
  void printOrderBookHistogram(int blockSize) const;

  friend class TestOrderBook;
};

#endif // !ORDERBOOK_INCLUDED
