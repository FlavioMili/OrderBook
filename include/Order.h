#ifndef ORDER_INCLUDED
#define ORDER_INCLUDED

#include <ctime>

enum class OrderSide { BUY, SELL };
struct Order {
   time_t timestamp;
   OrderSide side;
   double price;
   int quantity;
   int ID;

   Order(time_t timestamp, OrderSide side, double price, int quantity, int ID)
   :timestamp(timestamp), side(side), price(price), quantity(quantity), ID(ID) {};

   Order() = default;

   bool operator<(const Order& other) const {
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

