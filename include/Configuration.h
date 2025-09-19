// ----------------------------------------------------------------------------- //
//                                                                               //
//  Order Book Simulator                                                         //
//  Copyright (c) 2025 Flavio Milinanni. All Rights Reserved.                    //
//                                                                               //
// ----------------------------------------------------------------------------- //


#ifndef CONFIGURATION_INCLUDED
#define CONFIGURATION_INCLUDED

#include <cstdint>
#include <string>
#include <vector>

namespace Config {

// === Order Book Configuration ===
constexpr double minPrice = 50.0;
constexpr double tickSize = 0.1;
constexpr size_t priceLevels = 501;

// === Order Pool Configuration ===
// A chunk size of 2^20 orders. 1,048,576 orders * 24 bytes/order = ~25MB per chunk.
constexpr size_t orderPoolChunkSize = 1048576;

// === Data Generator Configuration ===
const std::vector<std::string> tickers = {
  "AAPL", "MSFT", "GOOG", "AMZN", "TSLA", "NVDA", "META", "JPM", "V", "JNJ"
};
constexpr double minGenPrice = 50.0;
constexpr double maxGenPrice = 500.0;
constexpr int minGenQty = 1;
constexpr int maxGenQty = 100;
constexpr uint32_t initialOrderId = 1000;
constexpr uint64_t initialTimestamp = 1694778123456789;

// === Data Generation Distribution (as percentages) ===
constexpr int addInstructionWeight = 60; // 60% ADD instructions
constexpr int cancelInstructionWeight = 20; // 20% CANCEL instructions
constexpr int editInstructionWeight = 20; // 20% EDIT instructions

// Probability that a CANCEL or EDIT instruction will target an already-removed order,
// thus intentionally creating a failure to test robustness.
constexpr bool staleInstruction = true;
constexpr double staleInstructionProbability = 1.0 / 50'000'000;

// === Benchmark Configuration ===
const std::string dataFileName = "orders.dat";
constexpr int numInstructions = 1'000'000'000;
constexpr int histogramBlockSize = 10'000'000;

} // namespace Config

#endif // !CONFIGURATION_INCLUDED
