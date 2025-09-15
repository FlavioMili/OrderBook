#include "../include/OrderBook.h"
#include <iomanip>
#include <iostream>

OrderBook::OrderBook() : orderPool() {
  BuyOrders.resize(priceLevels);
  SellOrders.resize(priceLevels);
}

void OrderBook::updateBestBid() {
  for (int i = bestBidIndex; i >= 0; --i) {
    if (!BuyOrders[i].empty()) {
      bestBidIndex = i;
      return;
    }
  }
  bestBidIndex = -1; // No bids left
}

void OrderBook::updateBestAsk() {
  for (size_t i = (bestAskIndex == -1) ? 0 : bestAskIndex; i < priceLevels; ++i) {
    if (!SellOrders[i].empty()) {
      bestAskIndex = i;
      return;
    }
  }
  bestAskIndex = -1; // No asks left
}

void OrderBook::removeFrontOrder(bool isBuySide, size_t index) {
  auto& orders = isBuySide ? BuyOrders[index] : SellOrders[index];
  orderPool.deallocate(orders.front());
  orders.pop_front();

  if (orders.empty()) {
    if (isBuySide && static_cast<int>(index) == bestBidIndex) {
      updateBestBid();
    } else if (!isBuySide && static_cast<int>(index) == bestAskIndex) {
      updateBestAsk();
    }
  }
}

void OrderBook::processBuyMatching(uint32_t& quantity, size_t index) {
  while (quantity > 0 && bestAskIndex != -1 && index >= static_cast<size_t>(bestAskIndex)) {
    auto& sellOrdersAtPrice = SellOrders[bestAskIndex];
    Order* sellOrder = sellOrdersAtPrice.front();

    if (quantity < sellOrder->quantity) {
      sellOrder->quantity -= quantity;
      quantity = 0;
    } else {
      quantity -= sellOrder->quantity;
      removeFrontOrder(false, bestAskIndex);
    }
  }
}

void OrderBook::processSellMatching(uint32_t& quantity, size_t index) {
  while (quantity > 0 && bestBidIndex != -1 && index <= static_cast<size_t>(bestBidIndex)) {
    auto& buyOrdersAtPrice = BuyOrders[bestBidIndex];
    Order* buyOrder = buyOrdersAtPrice.front();

    if (quantity < buyOrder->quantity) {
      buyOrder->quantity -= quantity;
      quantity = 0;
    } else {
      quantity -= buyOrder->quantity;
      removeFrontOrder(true, bestBidIndex);
    }
  }
}

void OrderBook::processOrders(bool isBuy, double price, uint32_t quantity, uint32_t timestamp, uint32_t ID) {
  if (price < minPrice || price > (minPrice + (priceLevels - 1) * tickSize)) {
    return; // Price is out of the supported range
  }
  size_t index = priceToIndex(price);

  if (isBuy) {
    processBuyMatching(quantity, index);
    if (quantity > 0) {
      Order* newOrder = orderPool.allocate(timestamp, isBuy, price, quantity, ID);
      BuyOrders[index].push_back(newOrder);
      if (bestBidIndex == -1 || index > static_cast<size_t>(bestBidIndex)) {
        bestBidIndex = index;
      }
    }
  } else { // Sell
    processSellMatching(quantity, index);
    if (quantity > 0) {
      Order* newOrder = orderPool.allocate(timestamp, isBuy, price, quantity, ID);
      SellOrders[index].push_back(newOrder);
      if (bestAskIndex == -1 || index < static_cast<size_t>(bestAskIndex)) {
        bestAskIndex = index;
      }
    }
  }
}

void OrderBook::printOrderBookHistogram(int blockSize) const {
  const std::string GREEN = "\033[32m";
  const std::string RED = "\033[31m";

  std::cout << "===== Buy Orders =====\n";
  for (int i = bestBidIndex; i >= 0; --i) {
    if (!BuyOrders[i].empty()) {
      uint64_t totalQuantity = 0;
      for (const auto& order : BuyOrders[i]) {
        totalQuantity += order->quantity;
      }
      int symbolCount = totalQuantity / blockSize;
      if (symbolCount > 0) {
        std::cout << std::fixed << std::setprecision(2) << indexToPrice(i) << ": " << GREEN;
        for (int j = 0; j < symbolCount; ++j) {
          std::cout << "\u2587";
        }
        std::cout << "\033[0m\n";
      }
    }
  }

  std::cout << "\n===== Sell Orders =====\n";
  if (bestAskIndex != -1) {
    for (size_t i = bestAskIndex; i < priceLevels; ++i) {
      if (!SellOrders[i].empty()) {
        uint64_t totalQuantity = 0;
        for (const auto& order : SellOrders[i]) {
          totalQuantity += order->quantity;
        }
        int symbolCount = totalQuantity / blockSize;
        if (symbolCount > 0) {
          std::cout << std::fixed << std::setprecision(2) << indexToPrice(i) << ": " << RED;
          for (int j = 0; j < symbolCount; ++j) {
            std::cout << "\u2587";
          }
          std::cout << "\033[0m\n";
        }
      }
    }
  }
}
