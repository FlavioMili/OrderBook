// ----------------------------------------------------------------------------- //
//                                                                               //
//  Order Book Simulator                                                         //
//  Copyright (c) 2025 Flavio Milinanni. All Rights Reserved.                    //
//                                                                               //
// ----------------------------------------------------------------------------- //


#include "../include/OrderPool.h"
#include "../include/Configuration.h"

OrderPool::OrderPool() {
  memory_chunks.reserve(16);
  free_list.reserve(Config::orderPoolChunkSize);
}

void OrderPool::grow() {
  auto new_chunk = std::make_unique<std::vector<Order>>(Config::orderPoolChunkSize);
  for (auto& order : *new_chunk) {
    free_list.push_back(&order);
  }
  memory_chunks.push_back(std::move(new_chunk));
}

Order* OrderPool::allocate(uint32_t timestamp, bool isBuy, double price, uint32_t quantity, uint32_t ID, uint32_t tickerId) {
  if (free_list.empty()) {
    grow();
  }

  Order* order = free_list.back();
  free_list.pop_back();

  order->price = price;
  order->ID = ID;
  order->tickerId = tickerId;
  order->timestamp = timestamp;
  order->quantity = quantity;
  order->isBuy = isBuy;
  order->next = nullptr;
  order->prev = nullptr;

  return order;
}

void OrderPool::deallocate(Order* order) {
  free_list.push_back(order);
}
