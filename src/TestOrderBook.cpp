#include "../include/OrderBook.h"
#include <ctime>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <cstdint>

void simulateMarket(OrderBook& book, long long numOrders) {
  srand(time(0));

  auto marketOpen = std::chrono::high_resolution_clock::now();
  for (uint32_t i = 0; i < numOrders; ++i) {
    bool isBuy = ((rand() & 1) == 0);
    double price = 50.0 + (rand() % 501) / 10.0;
    uint32_t quantity = rand() % 50 + 1;
    uint32_t timestamp = static_cast<uint32_t>(time(nullptr));
    uint32_t ID = i + 1;
    book.processOrders(isBuy, price, quantity, timestamp, ID);
  }
  auto marketClose = std::chrono::high_resolution_clock::now();
  auto totalTime = std::chrono::duration_cast<std::chrono::nanoseconds>(marketClose - marketOpen).count();

  // Print histogram at the end
  int blockSize = std::max(1LL, numOrders / 300);
  book.printOrderBookHistogram(blockSize);

  double averageTime = static_cast<double>(totalTime) / numOrders;
  std::cout << "\n--- Simulation Finished ---\n";
  std::cout << "Average time to process an order: " << averageTime << " nanoseconds\n";
  std::cout << "Total time to process " << numOrders << " orders: "
    << static_cast<double>(totalTime) / 1e9 << " seconds\n";
}

int main() {
  std::ios_base::sync_with_stdio(false);
  long long numOrders;
  std::cout << "Number of orders to process: ";
  std::cin >> numOrders;

  if (numOrders < 0) numOrders = 0;

  OrderBook book;

  simulateMarket(book, numOrders);

  return 0;
}
