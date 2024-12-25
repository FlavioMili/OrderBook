#ifndef ORDERBOOK_INCLUDED
#define ORDERBOOK_INCLUDED

#include "Order.h"
#include <limits>
#include <map>
#include <utility>
#include <vector>
#include <iostream>

class TestOrderBook;

class OrderBook {
private:
   std::map<double, std::vector<Order>, std::greater<double>> BuyOrders;
   std::map<double, std::vector<Order>> SellOrders;
   std::pair<double, Order*> bidMax {};
   std::pair<double, Order*> askMin {std::numeric_limits<double>::infinity(), nullptr};

public: 
   void addOrder(time_t timestamp, OrderSide side, double price, int quantity, int ID);
   void processOrders(OrderSide side, double price, int quantity, time_t timestamp, int ID);
   void printOrderBook();
   bool cancelOrder(int orderID);
   bool modifyOrder(int orderID, double newPrice, int newQuantity);
const std::map<double, std::vector<Order>, std::greater<double>>& getBuyOrders() const {
       return BuyOrders;
   }

   const std::map<double, std::vector<Order>>& getSellOrders() const {
       return SellOrders;
   }
   friend class TestOrderBook;
};

#endif // !ORDERBOOK_INCLUDED
