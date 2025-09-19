// ----------------------------------------------------------------------------- //
//                                                                               //
//  Order Book Simulator                                                         //
//  Copyright (c) 2025 Flavio Milinanni. All Rights Reserved.                    //
//                                                                               //
// ----------------------------------------------------------------------------- //


#ifndef ORDER_INCLUDED
#define ORDER_INCLUDED

#include <cstdint>

struct Order {
  double price;      // 8 bytes
  uint32_t ID;       // 4 bytes
  uint32_t tickerId;   // 4 bytes
  uint32_t timestamp;// 4 bytes
  uint32_t quantity; // 4 bytes
  bool isBuy;        // 1 byte

  // Pointers for doubly-linked list
  Order* next = nullptr;
  Order* prev = nullptr;

  Order() = default;
}; 

#endif // !ORDER_INCLUDED
