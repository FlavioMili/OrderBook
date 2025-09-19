// ----------------------------------------------------------------------------- //
//                                                                               //
//  Order Book Simulator                                                         //
//  Copyright (c) 2025 Flavio Milinanni. All Rights Reserved.                    //
//                                                                               //
// ----------------------------------------------------------------------------- //


#ifndef MATCHING_ENGINE_INCLUDED
#define MATCHING_ENGINE_INCLUDED

#include "OrderBook.h"
#include <string>
#include <memory> // Required for std::unique_ptr
#include <vector>

class MatchingEngine {
private:
  std::vector<std::unique_ptr<OrderBook>> orderBooks;
  std::vector<std::string> tickerIdToNameMap;

public:
  MatchingEngine();
  void processOrders(uint32_t tickerId, bool isBuy, double price, uint32_t quantity, uint32_t timestamp, uint32_t ID);
  bool cancelOrder(uint32_t tickerId, uint32_t ID);
  bool editOrder(uint32_t tickerId, uint32_t ID, double newPrice, uint32_t newQuantity);
  void setTickerName(uint32_t tickerId, const std::string& tickerName);
  const OrderBook* getOrderBook(uint32_t tickerId) const;
  void printAllHistograms(int blockSize) const;
};
#endif // !MATCHING_ENGINE_INCLUDED
