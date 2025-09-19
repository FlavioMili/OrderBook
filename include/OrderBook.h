// ----------------------------------------------------------------------------- //
//                                                                               //
//  Order Book Simulator                                                         //
//  Copyright (c) 2025 Flavio Milinanni. All Rights Reserved.                    //
//                                                                               //
// ----------------------------------------------------------------------------- //


#include "Order.h"
#include "OrderPool.h"
#include "Configuration.h"
#include "FastMap.h"
#include <vector>
#include <cmath>
#include <string>

class TestOrderBook;

class OrderBook {
private:
  OrderPool orderPool;
  std::vector<Order*> BuyHeads; // Head of the doubly-linked list for buy orders at each price level
  std::vector<Order*> BuyTails; // Tail of the doubly-linked list for buy orders at each price level
  std::vector<Order*> SellHeads; // Head of the doubly-linked list for sell orders at each price level
  std::vector<Order*> SellTails; // Tail of the doubly-linked list for sell orders at each price level
  FastMap orderMap;

  int bestBidIndex = -1;
  int bestAskIndex = -1;

  size_t priceToIndex(double price) const {
    return static_cast<size_t>(std::round((price - Config::minPrice) / Config::tickSize));
  }

  double indexToPrice(size_t index) const {
    return Config::minPrice + (index * Config::tickSize);
  }

  void updateBestBid();
  void updateBestAsk();
  void processBuyMatching(uint32_t& quantity, size_t index);
  void processSellMatching(uint32_t& quantity, size_t index);
  void removeFrontOrder(bool isBuySide, size_t index);
  void removeOrderFromList(Order* order);

public:
  OrderBook();
  OrderBook(const OrderBook&) = delete;
  OrderBook& operator=(const OrderBook&) = delete;
  OrderBook(OrderBook&&) = delete;
  OrderBook& operator=(OrderBook&&) = delete;

  void processOrders(bool isBuy, double price, uint32_t quantity, uint32_t timestamp, uint32_t ID, uint32_t tickerId);
  bool cancelOrder(uint32_t ID);
  bool editOrder(uint32_t ID, double newPrice, uint32_t newQuantity);
  void printOrderBookHistogram(const std::string& tickerName, int blockSize) const;

  friend class TestOrderBook;
};
