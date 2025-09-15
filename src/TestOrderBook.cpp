#include "../include/OrderBook.h"
#include <ctime>  
#include <chrono>  
#include <iostream>

void printOrderBookHistogram(const OrderBook& book, long long numOrders) {
  const std::string GREEN = "\033[32m";
  const std::string RED = "\033[31m";
  int blockSize = std::max(1LL, numOrders / 300);

  std::cout << "===== Buy Orders =====\n";
  OrderBook::printHistogram(book.getBuyOrders(), blockSize, GREEN);

  std::cout << "\n===== Sell Orders =====\n";
  OrderBook::printHistogram(book.getSellOrders(), blockSize, RED);
}

void simulateMarket(OrderBook& book, long long numOrders) {
  srand(time(0));

  long long totalTime = 0; 
  for (int i = 0; i < numOrders; ++i) {
    auto start = std::chrono::high_resolution_clock::now();
    OrderSide side = ((rand() & 2) == 0) ? OrderSide::BUY : OrderSide::SELL;
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
  std::cout << "Total time to process " << numOrders << " orders: "
    << static_cast<double>(totalTime) / 1e9 << " seconds\n";
}

int main() {
  std::ios_base::sync_with_stdio(false);
  long long numOrders;
  std::cout << "Number of orders to process: ";
  std::cin >> numOrders;
  OrderBook book(numOrders);
  simulateMarket(book, numOrders);
  return 0;
}