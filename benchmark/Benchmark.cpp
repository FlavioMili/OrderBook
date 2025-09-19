// ----------------------------------------------------------------------------- //
//                                                                               //
//  Order Book Simulator                                                         //
//  Copyright (c) 2025 Flavio Milinanni. All Rights Reserved.                    //
//                                                                               //
// ----------------------------------------------------------------------------- //

#include <benchmark/benchmark.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <thread>
#include <iostream>

#include "../include/MatchingEngine.h"
#include "../include/Configuration.h"
#include "../include/Reporting.h"
#include "../include/TickerResult.h"

std::vector<TickerResult> latest_results;

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

void process_ticker_file(MatchingEngine& engine, uint32_t tickerId, const std::string& filename, TickerResult& result) {
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1) return;

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        close(fd);
        return;
    }
    size_t file_size = sb.st_size;

    if (file_size == 0) {
        close(fd);
        return;
    }

    const char* mapped_file = static_cast<const char*>(mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0));
    if (mapped_file == MAP_FAILED) {
        close(fd);
        return;
    }
    close(fd);

    const char* p = mapped_file;
    const char* end = mapped_file + file_size;

    auto start_time = std::chrono::high_resolution_clock::now();

    while (p < end) {
        uint32_t id, qty;
        double price;
        char type, side_char;

        p = fast_atoi(p, id);
        p++; // Skip ';'

        while (*p != ';') p++; // Skip ticker
        p++; // Skip ';'

        side_char = *p;
        p += 2; // Skip side and ';'

        p = fast_atof(p, price);
        p++; // Skip ';'

        p = fast_atoi(p, qty);
        p++; // Skip ';'

        type = *p;
        
        while (p < end && *p != '\n') p++;
        p++; // Skip newline

        if (type == 'A') {
            result.add_count++;
            engine.processOrders(tickerId, side_char == 'B', price, qty, 0, id);
        } else if (type == 'C') {
            result.cancel_count++;
            if (!engine.cancelOrder(tickerId, id)) {
                result.failed_cancels++;
            }
        } else if (type == 'E') {
            result.edit_count++;
            if (!engine.editOrder(tickerId, id, price, qty)) {
                result.failed_edits++;
            }
        }
        result.instruction_count++;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    result.time_ms = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.0;

    munmap((void*)mapped_file, file_size);
}

static void BM_OrderProcessing(benchmark::State& state) {
    for (auto _ : state) {
        MatchingEngine engine;
        for (uint32_t i = 0; i < Config::tickers.size(); ++i) {
            engine.setTickerName(i, Config::tickers[i]);
        }

        std::vector<std::thread> threads;
        std::vector<TickerResult> results(Config::tickers.size());

        for (uint32_t i = 0; i < Config::tickers.size(); ++i) {
            results[i].name = Config::tickers[i];
            std::string filename = Config::tickers[i] + ".dat";
            threads.emplace_back(process_ticker_file, std::ref(engine), i, filename, std::ref(results[i]));
        }

        for (auto& t : threads) {
            t.join();
        }

        long long total_instructions = 0;
        for(const auto& r : results) total_instructions += r.instruction_count;
        state.SetItemsProcessed(total_instructions);
        latest_results = std::move(results);
    }
}

BENCHMARK(BM_OrderProcessing)->Unit(benchmark::kMillisecond);

int main(int argc, char** argv) {
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
    benchmark::RunSpecifiedBenchmarks();
    print_table(latest_results);
    return 0;
}
