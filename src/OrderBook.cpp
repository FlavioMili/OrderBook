// ----------------------------------------------------------------------------- //
//                                                                               //
//  Order Book Simulator                                                         //
//  Copyright (c) 2025 Flavio Milinanni. All Rights Reserved.                    //
//                                                                               //
// ----------------------------------------------------------------------------- //


#include "../include/OrderBook.h"
#include "../include/Configuration.h"
#include <iomanip>
#include <iostream>
#include <algorithm>

OrderBook::OrderBook() : orderPool() {
  bestBidIndex = -1;
  bestAskIndex = -1;
  BuyHeads.resize(Config::priceLevels, nullptr);
  BuyTails.resize(Config::priceLevels, nullptr);
  SellHeads.resize(Config::priceLevels, nullptr);
  SellTails.resize(Config::priceLevels, nullptr);
}

void OrderBook::updateBestBid() {
  for (int i = bestBidIndex; i >= 0; --i) {
    if (BuyHeads[i] != nullptr) {
      bestBidIndex = i;
      return;
    }
  }
  bestBidIndex = -1; // No bids left
}

void OrderBook::updateBestAsk() {
  for (size_t i = (bestAskIndex == -1) ? 0 : bestAskIndex; i < Config::priceLevels; ++i) {
    if (SellHeads[i] != nullptr) {
      bestAskIndex = i;
      return;
    }
  }
  bestAskIndex = -1; // No asks left
}

// Helper to remove an order from its linked list
void OrderBook::removeOrderFromList(Order* order) {
  size_t index = priceToIndex(order->price);
  Order** head = order->isBuy ? &BuyHeads[index] : &SellHeads[index];
  Order** tail = order->isBuy ? &BuyTails[index] : &SellTails[index];

  if (order->prev) {
    order->prev->next = order->next;
  } else { // This was the head
    *head = order->next;
  }

  if (order->next) {
    order->next->prev = order->prev;
  } else { // This was the tail
    *tail = order->prev;
  }

  order->next = nullptr;
  order->prev = nullptr;

  // Update best bid/ask if the removed order was at the best level and it's now empty
  if (*head == nullptr) { // List became empty
    if (order->isBuy && static_cast<int>(index) == bestBidIndex) {
      updateBestBid();
    } else if (!order->isBuy && static_cast<int>(index) == bestAskIndex) {
      updateBestAsk();
    }
  }
}

void OrderBook::processBuyMatching(uint32_t& quantity, size_t index) {
  while (quantity > 0 && bestAskIndex != -1 && index >= static_cast<size_t>(bestAskIndex)) {
    Order* sellOrder = SellHeads[bestAskIndex];

    if (quantity < sellOrder->quantity) {
      sellOrder->quantity -= quantity;
      quantity = 0;
    } else {
      quantity -= sellOrder->quantity;
      orderMap.erase(sellOrder->ID);
      removeOrderFromList(sellOrder);
      orderPool.deallocate(sellOrder);
    }
  }
}

void OrderBook::processSellMatching(uint32_t& quantity, size_t index) {
  while (quantity > 0 && bestBidIndex != -1 && index <= static_cast<size_t>(bestBidIndex)) {
    Order* buyOrder = BuyHeads[bestBidIndex];

    if (quantity < buyOrder->quantity) {
      buyOrder->quantity -= quantity;
      quantity = 0;
    } else {
      quantity -= buyOrder->quantity;
      orderMap.erase(buyOrder->ID);
      removeOrderFromList(buyOrder);
      orderPool.deallocate(buyOrder);
    }
  }
}

void OrderBook::processOrders(bool isBuy, double price, uint32_t quantity, uint32_t timestamp, uint32_t ID, uint32_t tickerId) {
  if (price < Config::minPrice || price > (Config::minPrice + (Config::priceLevels - 1) * Config::tickSize)) {
    return; // Price is out of the supported range
  }
  size_t index = priceToIndex(price);

  if (isBuy) {
    processBuyMatching(quantity, index);
    if (quantity > 0) {
      Order* newOrder = orderPool.allocate(timestamp, isBuy, price, quantity, ID, tickerId);
      if (BuyTails[index] != nullptr) {
        BuyTails[index]->next = newOrder;
        newOrder->prev = BuyTails[index];
      }
      BuyTails[index] = newOrder;
      if (BuyHeads[index] == nullptr) {
        BuyHeads[index] = newOrder;
      }
      orderMap[ID] = newOrder;
      if (bestBidIndex == -1 || index > static_cast<size_t>(bestBidIndex)) {
        bestBidIndex = index;
      }
    }
  } else { // Sell
    processSellMatching(quantity, index);
    if (quantity > 0) {
      Order* newOrder = orderPool.allocate(timestamp, isBuy, price, quantity, ID, tickerId);
      if (SellTails[index] != nullptr) {
        SellTails[index]->next = newOrder;
        newOrder->prev = SellTails[index];
      }
      SellTails[index] = newOrder;
      if (SellHeads[index] == nullptr) {
        SellHeads[index] = newOrder;
      }
      orderMap[ID] = newOrder;
      if (bestAskIndex == -1 || index < static_cast<size_t>(bestAskIndex)) {
        bestAskIndex = index;
      }
    }
  }
}

bool OrderBook::cancelOrder(uint32_t ID) {
  Order** order_ptr = orderMap.find(ID); 
  if (order_ptr == nullptr) {
    return false; // Order not found
  }

  Order* order = *order_ptr;
  removeOrderFromList(order);
  orderMap.erase(ID);
  orderPool.deallocate(order);

  return true;
}

bool OrderBook::editOrder(uint32_t ID, double newPrice, uint32_t newQuantity) {
  Order** order_ptr = orderMap.find(ID);
  if (order_ptr == nullptr) {
    return false; // Order not found
  }

  Order* order = *order_ptr;

  if (order->price != newPrice || newQuantity > order->quantity) {
    bool isBuy = order->isBuy;
    uint32_t timestamp = order->timestamp;
    uint32_t tickerId = order->tickerId;

    cancelOrder(ID);
    processOrders(isBuy, newPrice, newQuantity, timestamp, ID, tickerId);
  } else {
    order->quantity = newQuantity;
  }
  return true;
}

void OrderBook::printOrderBookHistogram(const std::string& tickerName, int blockSize) const {
  const std::string GREEN = "\033[32m";
  const std::string RED = "\033[31m";
  const std::string RESET = "\033[0m";

  std::cout << "\n--- Order Book for " << tickerName << " ---\n";
  std::cout << std::fixed << std::setprecision(2);
  std::cout << "+------------------------+-----------+------------------------+\n";
  std::cout << "|       BUY ORDERS       |   PRICE   |       SELL ORDERS      |\n";
  std::cout << "+------------------------+-----------+------------------------+\n";

  int max_level = 0;
  if (bestBidIndex != -1) max_level = std::max(max_level, bestBidIndex);
  if (bestAskIndex != -1) {
    for (int i = Config::priceLevels - 1; i >= bestAskIndex; --i) {
      if (SellHeads[i] != nullptr) {
        max_level = std::max(max_level, i);
        break;
      }
    }
  }

  for (int i = max_level; i >= 0; --i) {
    uint64_t buy_quantity = 0;
    if (BuyHeads[i] != nullptr) {
      Order* current = BuyHeads[i];
      while(current != nullptr) {
        buy_quantity += current->quantity;
        current = current->next;
      }
    }

    uint64_t sell_quantity = 0;
    if (i < (int)Config::priceLevels && SellHeads[i] != nullptr) {
      Order* current = SellHeads[i];
      while(current != nullptr) {
        sell_quantity += current->quantity;
        current = current->next;
      }
    }

    if (buy_quantity == 0 && sell_quantity == 0) {
      continue;
    }

    // Buy side
    std::cout << "| " << GREEN;
    if (buy_quantity > 0) {
      int bar_length = buy_quantity / blockSize;
      std::cout << std::right << std::setw(22) << std::string(bar_length, '#');
    } else {
      std::cout << std::setw(22) << "";
    }
    std::cout << RESET << " |";

    // Price
    std::cout << std::setw(10) << indexToPrice(i) << " | ";

    // Sell side
    std::cout << RED;
    if (sell_quantity > 0) {
      int bar_length = sell_quantity / blockSize;
      std::cout << std::left << std::setw(22) << std::string(bar_length, '#');
    } else {
      std::cout << std::setw(22) << "";
    }
    std::cout << RESET << " |";
  }

  std::cout << "+------------------------+-----------+------------------------+\n";
}
