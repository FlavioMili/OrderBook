#include "../include/Order.h"
#include "../include/OrderBook.h"
#include <memory>

OrderBook::OrderBook(size_t poolSize) : orderPool(poolSize) {}

void OrderBook::updateBidMax() {
  if (BuyOrders.empty()) {
    bidMax = {-std::numeric_limits<double>::infinity(), nullptr};
  } else {
    auto it = BuyOrders.begin();
    bidMax.price = it->first;
    bidMax.order = it->second.front();
  }
}

void OrderBook::updateAskMin() {
  if (SellOrders.empty()) {
    askMin = {std::numeric_limits<double>::infinity(), nullptr};
  } else {
    auto it = SellOrders.begin();
    askMin.price = it->first;
    askMin.order = it->second.front();
  }
}

void OrderBook::processBuyMatching(double price, int& quantity) {
  while (quantity > 0 && !SellOrders.empty() && price >= askMin.price) {
    auto& sellOrdersAtPrice = SellOrders[askMin.price];
    Order* sellOrder = sellOrdersAtPrice.front();

    if (quantity < sellOrder->quantity) {
      sellOrder->quantity -= quantity;
      quantity = 0;
    } else {
      quantity -= sellOrder->quantity;
      removeFrontOrder(SellOrders, askMin.price);
      updateAskMin();
    }
  }
}

void OrderBook::processSellMatching(double price, int& quantity) {
  while (quantity > 0 && !BuyOrders.empty() && price <= bidMax.price) {
    auto& buyOrdersAtPrice = BuyOrders[bidMax.price];
    Order* buyOrder = buyOrdersAtPrice.front();

    if (quantity < buyOrder->quantity) {
      buyOrder->quantity -= quantity;
      quantity = 0;
    } else {
      quantity -= buyOrder->quantity;
      removeFrontOrder(BuyOrders, bidMax.price);
      updateBidMax();
    }
  }
}

void OrderBook::processOrders(OrderSide side, double price, int quantity, time_t timestamp, int ID) {
  if (side == OrderSide::BUY) {
    processBuyMatching(price, quantity);
    if (quantity > 0) {
      Order* newOrder = orderPool.allocate(timestamp, OrderSide::BUY, price, quantity, ID);
      if (BuyOrders.empty() || price > bidMax.price) {
        bidMax = {price, newOrder};
      }
      BuyOrders[price].push_back(newOrder);
    }
  } else {
    processSellMatching(price, quantity);
    if (quantity > 0) {
      Order* newOrder = orderPool.allocate(timestamp, OrderSide::SELL, price, quantity, ID);
      if (SellOrders.empty() || price < askMin.price) {
        askMin = {price, newOrder};
      }
      SellOrders[price].push_back(newOrder);
    }
  }
}

void OrderBook::printOrderBook() {
  std::cout << "Buy Orders:\n";
  printOrders(BuyOrders);

  std::cout << "Sell Orders:\n";
  printOrders(SellOrders);
}