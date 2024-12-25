#include "../include/Order.h"
#include "../include/OrderBook.h"

void OrderBook::processOrders(OrderSide side, double price, int quantity, time_t timestamp, int ID){
   if (side == OrderSide::BUY) { 
      while (quantity > 0 && !SellOrders.empty() && price >= askMin.first) {
         auto& sellOrdersAtPrice = SellOrders[askMin.first];
         Order* sellOrder = &sellOrdersAtPrice.front();

         if (price >= sellOrder->price) {
            if (quantity < sellOrder->quantity) {
            sellOrder->quantity -= quantity;
            quantity = 0;
         } else {
            quantity -= sellOrder->quantity;
            sellOrdersAtPrice.erase(sellOrdersAtPrice.begin()); 

            if (sellOrdersAtPrice.empty()) {
               SellOrders.erase(askMin.first);  

               if (!SellOrders.empty()) {
                  askMin.first = SellOrders.begin()->first;
                  askMin.second = &SellOrders[askMin.first].front();
               } else {
                  askMin.second = nullptr;
                  }
               }
            }
         }
      }

      if (quantity > 0) {
         BuyOrders[price].emplace_back(timestamp, OrderSide::BUY, price, quantity, ID);

         bidMax.first = price;
         bidMax.second = &BuyOrders[price].back();
      }
   } else {  // SELL
      while (quantity > 0 && !BuyOrders.empty() && price <= bidMax.first) {
         auto& buyOrdersAtPrice = BuyOrders[bidMax.first];
         Order* buyOrder = &buyOrdersAtPrice.front();

         if (price <= buyOrder->price) {
            if (quantity < buyOrder->quantity) {
            buyOrder->quantity -= quantity;
            quantity = 0;
         } else {
            quantity -= buyOrder->quantity;
            buyOrdersAtPrice.erase(buyOrdersAtPrice.begin());
            if (buyOrdersAtPrice.empty()) {
               BuyOrders.erase(bidMax.first);

               if (!BuyOrders.empty()) {
                  bidMax.first = BuyOrders.begin()->first;
                  bidMax.second = &BuyOrders[bidMax.first].front();
               } else {
                  bidMax.second = nullptr;
                  }
               }
            }
         }
      }

      if (quantity > 0) {
      SellOrders[price].emplace_back(timestamp, OrderSide::SELL, price, quantity, ID);
      askMin.first = price;
      askMin.second = &SellOrders[price].back();
      }
   }
}

void OrderBook::printOrderBook() {
   std::cout << "Buy Orders:\n";
   for (const auto& [price, orders] : BuyOrders) {
      for (const auto& order : orders) {
      std::cout << "ID: " << order.ID << ", Price: " << order.price << ", Quantity: " << order.quantity << "\n";
      }
   }

   std::cout << "Sell Orders:\n";
   for (const auto& [price, orders] : SellOrders) {
      for (const auto& order : orders) {
         std::cout << "ID: " << order.ID << ", Price: " << order.price << ", Quantity: " << order.quantity << "\n";
      }
   }
}
