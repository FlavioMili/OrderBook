#ifndef ORDER_INCLUDED
#define ORDER_INCLUDED

#include <cstdint>

struct Order {
  double price;      // 8 bytes
  uint32_t ID;       // 4 bytes
  uint32_t timestamp;// 4 bytes
  uint32_t quantity; // 4 bytes
  bool isBuy;        // 1 byte

  Order() = default;

  // The comparison operator is no longer trivial due to the bool.
  // It's also not used by the high-performance array-based order book,
  // so it can be removed.
}; 

#endif // !ORDER_INCLUDED