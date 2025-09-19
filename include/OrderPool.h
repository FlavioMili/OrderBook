// ----------------------------------------------------------------------------- //
//                                                                               //
//  Order Book Simulator                                                         //
//  Copyright (c) 2025 Flavio Milinanni. All Rights Reserved.                    //
//                                                                               //
// ----------------------------------------------------------------------------- //


#ifndef ORDER_POOL_INCLUDED
#define ORDER_POOL_INCLUDED

#include "../include/Order.h"
#include <vector>
#include <memory>

class OrderPool {
public:
  OrderPool();
  void deallocate(Order* order);
  Order* allocate(uint32_t timestamp, bool isBuy, double price, uint32_t quantity, uint32_t ID, uint32_t tickerId);

private:
  void grow();

  std::vector<std::unique_ptr<std::vector<Order>>> memory_chunks;
  std::vector<Order*> free_list;
};

#endif // !ORDER_POOL_INCLUDED
