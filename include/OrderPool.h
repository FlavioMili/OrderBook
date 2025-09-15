#ifndef ORDER_POOL_INCLUDED
#define ORDER_POOL_INCLUDED

#include "../include/Order.h"
#include <vector>
#include <memory>

class OrderPool {
public:
  OrderPool();
  Order* allocate(uint32_t timestamp, bool isBuy, double price, uint32_t quantity, uint32_t ID);
  void deallocate(Order* order);

private:
  void grow();

  // A chunk size of 2^20 orders. 
  // 1,048,576 orders * 24 bytes/order = ~25MB per chunk.
  static constexpr size_t CHUNK_SIZE = 1048576;

  std::vector<std::unique_ptr<std::vector<Order>>> memory_chunks;
  std::vector<Order*> free_list;
};

#endif // !ORDER_POOL_INCLUDED
