// ----------------------------------------------------------------------------- //
//                                                                               //
//  Order Book Simulator                                                         //
//  Copyright (c) 2025 Flavio Milinanni. All Rights Reserved.                    //
//                                                                               //
// ----------------------------------------------------------------------------- //

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../include/MatchingEngine.h"
#include "../include/Configuration.h"

MatchingEngine::MatchingEngine() {
    orderBooks.resize(Config::tickers.size());
    tickerIdToNameMap.resize(Config::tickers.size());
    for (size_t i = 0; i < Config::tickers.size(); ++i) {
        orderBooks[i] = std::make_unique<OrderBook>();
    }
}

void MatchingEngine::processOrders(uint32_t tickerId, bool isBuy, double price, uint32_t quantity, uint32_t timestamp, uint32_t ID) {
  if (tickerId >= orderBooks.size()) return;
  orderBooks[tickerId]->processOrders(isBuy, price, quantity, timestamp, ID, tickerId);
}
bool MatchingEngine::cancelOrder(uint32_t tickerId, uint32_t ID) {
  if (tickerId >= orderBooks.size() || !orderBooks[tickerId]) return false;
  return orderBooks[tickerId]->cancelOrder(ID);
}

bool MatchingEngine::editOrder(uint32_t tickerId, uint32_t ID, double newPrice, uint32_t newQuantity) {
  if (tickerId >= orderBooks.size() || !orderBooks[tickerId]) return false;
  return orderBooks[tickerId]->editOrder(ID, newPrice, newQuantity);
}

void MatchingEngine::setTickerName(uint32_t tickerId, const std::string& tickerName) {
  if (tickerId >= tickerIdToNameMap.size()) return;
  tickerIdToNameMap[tickerId] = tickerName;
}

const OrderBook* MatchingEngine::getOrderBook(uint32_t tickerId) const {
  if (tickerId >= orderBooks.size() || !orderBooks[tickerId]) return nullptr;
  return orderBooks[tickerId].get();
}

void MatchingEngine::printAllHistograms(int blockSize) const {
  std::cout << "\n--- Final Order Book State ---\n";
  for (size_t i = 0; i < orderBooks.size(); ++i) {
    if (orderBooks[i]) {
        std::string tickerName = "Unknown";
        if (i < tickerIdToNameMap.size() && !tickerIdToNameMap[i].empty()) {
            tickerName = tickerIdToNameMap[i];
        }
        std::cout << "\n--- Ticker ID: " << i << " (" << tickerName << ") ---\n";
        orderBooks[i]->printOrderBookHistogram(tickerName, blockSize);
    }
  }
}
