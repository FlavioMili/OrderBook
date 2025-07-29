#include "../include/Order.h"
#include "../include/OrderBook.h"

void OrderBook::updateBidMax() {
  if (BuyOrders.empty()) {
    bidMax = {-std::numeric_limits<double>::infinity(), nullptr};
  } else {
    auto it = BuyOrders.begin();
    bidMax.first = it->first;
    bidMax.second = it->second.front();
  }
}

void OrderBook::updateAskMin() {
  if (SellOrders.empty()) {
    askMin = {std::numeric_limits<double>::infinity(), nullptr};
  } else {
    auto it = SellOrders.begin();
    askMin.first = it->first;
    askMin.second = it->second.front();
  }
}

void OrderBook::processBuyMatching(double price, int& quantity) {
  while (quantity > 0 && !SellOrders.empty() && price >= askMin.first) {
    auto& sellOrdersAtPrice = SellOrders[askMin.first];
    Order* sellOrder = sellOrdersAtPrice.front();

    if (quantity < sellOrder->quantity) {
      sellOrder->quantity -= quantity;
      quantity = 0;
    } else {
      quantity -= sellOrder->quantity;
      removeFrontOrder(SellOrders, askMin.first);
      updateAskMin();
    }
  }
}

void OrderBook::processSellMatching(double price, int& quantity) {
  while (quantity > 0 && !BuyOrders.empty() && price <= bidMax.first) {
    auto& buyOrdersAtPrice = BuyOrders[bidMax.first];
    Order* buyOrder = buyOrdersAtPrice.front();

    if (quantity < buyOrder->quantity) {
      buyOrder->quantity -= quantity;
      quantity = 0;
    } else {
      quantity -= buyOrder->quantity;
      removeFrontOrder(BuyOrders, bidMax.first);
      updateBidMax();
    }
  }
}

void OrderBook::processOrders(OrderSide side, double price, int quantity, time_t timestamp, int ID) {
  if (side == OrderSide::BUY) {
    processBuyMatching(price, quantity);
    if (quantity > 0) {
      Order* newOrder = new Order(timestamp, OrderSide::BUY, price, quantity, ID);
      BuyOrders[price].push_back(newOrder);
      if (BuyOrders.size() == 1 || price > bidMax.first) {
        bidMax = {price, newOrder};
      }
    }
  } else {
    processSellMatching(price, quantity);
    if (quantity > 0) {
      Order* newOrder = new Order(timestamp, OrderSide::SELL, price, quantity, ID);
      SellOrders[price].push_back(newOrder);
      if (SellOrders.size() == 1 || price < askMin.first) {
        askMin = {price, newOrder};
      }
    }
  }
}

void OrderBook::printOrderBook() {
  std::cout << "Buy Orders:\n";
  printOrders(BuyOrders);

  std::cout << "Sell Orders:\n";
  printOrders(SellOrders);
}
