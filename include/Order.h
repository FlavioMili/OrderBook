#ifndef ORDER_INCLUDED
#define ORDER_INCLUDED

#include <ctime>

enum class OrderSide { BUY, SELL };

struct Order {
  double price;
  time_t timestamp;
  int quantity;
  int ID;
  OrderSide side;

  Order(time_t timestamp, OrderSide side,
        double price, int quantity, int ID)
  :price(price), timestamp(timestamp), quantity(quantity),
    ID(ID), side(side) {};

  Order() = default;

  bool operator < (const Order& other) const {
    if (price == other.price) {
      return timestamp < other.timestamp;
    }
    if (other.side == OrderSide::BUY){
      return price > other.price;
    }
    return price < other.price; // SELL
  }
}; 

#endif // !ORDER_INCLUDED