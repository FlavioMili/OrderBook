// ----------------------------------------------------------------------------- //
//                                                                               //
//  Order Book Simulator                                                         //
//  Copyright (c) 2025 Flavio Milinanni. All Rights Reserved.                    //
//                                                                               //
// ----------------------------------------------------------------------------- //

#include <benchmark/benchmark.h>
#include <chrono>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include <iomanip>

#include "../include/MatchingEngine.h"
#include "../include/Configuration.h"

// Custom fast parser for positive integers
inline const char* fast_atoi(const char* p, uint32_t& out) {
  out = 0;
  while (*p >= '0' && *p <= '9') {
    out = out * 10 + (*p++ - '0');
  }
  return p;
}

// Custom fast parser for doubles
inline const char* fast_atof(const char* p, double& out) {
  double res = 0.0;
  double frac = 0.0;
  double div = 1.0;
  while (*p >= '0' && *p <= '9') {
    res = res * 10.0 + (*p++ - '0');
  }
  if (*p == '.') {
    p++;
    while (*p >= '0' && *p <= '9') {
      frac = frac * 10.0 + (*p++ - '0');
      div *= 10.0;
    }
    res += frac / div;
  }
  out = res;
  return p;
}

// Custom fast parser for tickers
inline const char* fast_ticker_atoi(const char* p, uint32_t& out) {
  // This is a simple but fast way to parse the known tickers
  switch (p[0]) {
    case 'A': out = 0; p += 4; break; // AAPL
    case 'M': out = 1; p += 4; break; // MSFT
    case 'G': out = 2; p += 4; break; // GOOG
    case 'T': out = 4; p += 4; break; // TSLA
    case 'N': out = 5; p += 4; break; // NVDA
    case 'J':
      if (p[1] == 'P') { out = 7; p += 3; break; } // JPM
      else { out = 9; p += 3; break; } // JNJ
    case 'V': out = 8; p += 1; break; // V
    default: // AMZN, META
      if (p[0] == 'A' && p[1] == 'M') { out = 3; p += 4; break; } // AMZN
      else { out = 6; p += 4; break; } // META
  }
  return p;
}


static void BM_OrderProcessing(benchmark::State& state) {
  int fd = open(Config::dataFileName.c_str(), O_RDONLY);
  if (fd == -1) {
    std::cerr << "Error: Could not open instruction file: " << Config::dataFileName << "\n";
    return;
  }

  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    close(fd);
    std::cerr << "Error: Could not get file size.\n";
    return;
  }
  size_t file_size = sb.st_size;

  const char* mapped_file = static_cast<const char*>(mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0));
  if (mapped_file == MAP_FAILED) {
    close(fd);
    std::cerr << "Error: Could not map file to memory.\n";
    return;
  }
  close(fd);

  // Skip header
  const char* p = static_cast<const char*>(memchr(mapped_file, '\n', file_size)) + 1;
  const char* end = mapped_file + file_size;

  long long add_count = 0;
  long long cancel_count = 0;
  long long edit_count = 0;
  long long failed_cancels = 0;
  long long failed_edits = 0;
  long long instruction_count = 0;

  auto start_time = std::chrono::high_resolution_clock::now();

  for (auto _ : state) {
    state.PauseTiming();
    MatchingEngine engine;
    for (uint32_t i = 0; i < Config::tickers.size(); ++i) {
      engine.setTickerName(i, Config::tickers[i]);
    }
    const char* current_p = p;
    instruction_count = 0;
    add_count = 0;
    cancel_count = 0;
    edit_count = 0;
    failed_cancels = 0;
    failed_edits = 0;
    state.ResumeTiming();

    while (current_p < end) {
      uint32_t id, qty, tickerId;
      double price;
      char type, side_char;

      current_p = fast_atoi(current_p, id);
      current_p++; // Skip ';'

      current_p = fast_ticker_atoi(current_p, tickerId);
      current_p++; // Skip ';'

      side_char = *current_p;
      current_p += 2; // Skip side and ';'

      current_p = fast_atof(current_p, price);
      current_p++; // Skip ';'

      current_p = fast_atoi(current_p, qty);
      current_p++; // Skip ';'

      type = *current_p;

      while (current_p < end && *current_p != '\n') current_p++;
      current_p++; // Skip newline

      if (type == 'A') {
        add_count++;
        engine.processOrders(tickerId, side_char == 'B', price, qty, 0, id);
      } else if (type == 'C') {
        cancel_count++;
        if (!engine.cancelOrder(tickerId, id)) {
          failed_cancels++;
        }
      } else if (type == 'E') {
        edit_count++;
        if (!engine.editOrder(tickerId, id, price, qty)) {
          failed_edits++;
        }
      }
      instruction_count++;
    }
    state.SetItemsProcessed(instruction_count);
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();

  munmap((void*)mapped_file, file_size);

  double total_seconds = elapsed_ns / 1e9;
  double instructions_per_second = (instruction_count > 0 && total_seconds > 0) ? instruction_count / total_seconds : 0;

  std::cout << "\n";
  std::cout << "+----------------------------------+\n";
  std::cout << "|      Benchmark Run Summary       |\n";
  std::cout << "+----------------------------------+\n";
  std::cout << "| Total Instructions: | " << std::setw(12) << instruction_count << " |\n";
  std::cout << "| Time Taken:         | " << std::setw(12) << std::fixed << std::setprecision(2) << total_seconds * 1000 << " ms |\n";
  std::cout << "| Instructions/sec:   | " << std::setw(12) << std::fixed << std::setprecision(2) << instructions_per_second / 1e6 << " M/s |\n";
  std::cout << "| Avg. Latency/Inst:  | " << std::setw(12) << std::fixed << std::setprecision(2) << (instruction_count > 0 ? (elapsed_ns / instruction_count) : 0) << " ns |\n";
  std::cout << "+----------------------------------+\n";
  std::cout << "| Instruction Mix:                 |\n";
  std::cout << "|   ADDs:             | " << std::setw(12) << add_count << " |\n";
  std::cout << "|   CANCELs:          | " << std::setw(12) << cancel_count << " |\n";
  std::cout << "|   EDITs:            | " << std::setw(12) << edit_count << " |\n";
  std::cout << "+----------------------------------+\n";
  std::cout << "| Operation Failures:              |\n";
  std::cout << "|   Failed CANCELs:   | " << std::setw(12) << failed_cancels << " |\n";
  std::cout << "|   Failed EDITs:     | " << std::setw(12) << failed_edits << " |\n";
  std::cout << "+----------------------------------+\n";
}

BENCHMARK(BM_OrderProcessing);

BENCHMARK_MAIN();
