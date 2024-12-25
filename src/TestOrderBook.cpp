#include <iostream>
#include <chrono>  
#include "../include/OrderBook.h"
#include <ctime>  


void printOrderBookHistogram(const OrderBook& book, long long numOrders) {
   const std::string RESET = "\033[0m";
   const std::string GREEN = "\033[32m"; 
   const std::string RED = "\033[31m";  
   int blockSize = std::max(1LL, numOrders/300);

   std::cout << "===== Buy Orders =====\n";
   for (const auto& [price, orders] : book.getBuyOrders()) {
      int totalQuantity = 0;
      for (const auto& order : orders) {
         totalQuantity += order.quantity;
      }
      int symbolCount = totalQuantity / blockSize; 

      std::cout << price << ": " << GREEN;
      for (int i = 0; i < symbolCount; ++i) {
         std::cout << "\u2587";
      }
      std::cout << RESET << "\n";
   }

   std::cout << "\n===== Sell Orders =====\n";
   for (const auto& [price, orders] : book.getSellOrders()) {
      int totalQuantity = 0;
      for (const auto& order : orders) {
         totalQuantity += order.quantity;
      }
      int symbolCount = totalQuantity / blockSize; 

      std::cout << price << ": " << RED;
      for (int i = 0; i < symbolCount; ++i) {
         std::cout << "\u2587";
      }
      std::cout << RESET << "\n";
   }
}

void simulateMarket(OrderBook& book, long long numOrders) {
   srand(time(0));

   long long totalTime = 0; 
   for (int i = 0; i < numOrders; ++i) {
      auto start = std::chrono::high_resolution_clock::now();
      OrderSide side = (rand() % 2 == 0) ? OrderSide::BUY : OrderSide::SELL;
      double price = 50.0 + (rand() % 501) / 10.0;
      int quantity = rand() % 50 + 1;
      time_t timestamp = time(nullptr);
      int ID = i + 1;
      if (side == OrderSide::BUY) {
         book.processOrders(OrderSide::BUY, price, quantity, timestamp, ID);
      }
      else if (side == OrderSide::SELL) {
         book.processOrders(OrderSide::SELL, price, quantity, timestamp, ID);
      }
      auto end = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
      totalTime += duration; 
   }
   printOrderBookHistogram(book, numOrders);
   double averageTime = static_cast<double>(totalTime) / numOrders;
   std::cout << "Average time to process an order: " << averageTime << " nanoseconds\n";
   std::cout << "Total time: " << static_cast<double>(totalTime) / 1e9 << " seconds\n";
}

int main() {
   std::ios_base::sync_with_stdio(false);
   OrderBook book;
   long long numOrders;
   std::cout << "Number of orders to process: ";
   std::cin >> numOrders;
   simulateMarket(book, numOrders);
   return 0;
}
