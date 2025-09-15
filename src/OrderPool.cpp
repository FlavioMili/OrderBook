#include "../include/OrderPool.h"
#include <stdexcept>

OrderPool::OrderPool() {
  memory_chunks.reserve(16);
  free_list.reserve(CHUNK_SIZE);
}

void OrderPool::grow() {
  auto new_chunk = std::make_unique<std::vector<Order>>(CHUNK_SIZE);
  for (auto& order : *new_chunk) {
    free_list.push_back(&order);
  }
  memory_chunks.push_back(std::move(new_chunk));
}

Order* OrderPool::allocate(uint32_t timestamp, bool isBuy, double price, uint32_t quantity, uint32_t ID) {
  if (free_list.empty()) {
    grow();
  }

  Order* order = free_list.back();
  free_list.pop_back();

  order->price = price;
  order->ID = ID;
  order->timestamp = timestamp;
  order->quantity = quantity;
  order->isBuy = isBuy;

  return order;
}

void OrderPool::deallocate(Order* order) {
  free_list.push_back(order);
}
