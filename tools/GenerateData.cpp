// ----------------------------------------------------------------------------- //
//                                                                               //
//  Order Book Simulator                                                         //
//  Copyright (c) 2025 Flavio Milinanni. All Rights Reserved.                    //
//                                                                               //
// ----------------------------------------------------------------------------- //

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <iomanip>
#include <thread>
#include <atomic>
#include <chrono>
#include <filesystem>

#include "../include/Configuration.h"

// The producer thread: generates instructions for a single ticker and writes to its own file.
void generator_thread_func(int thread_id, int num_instructions, uint32_t initial_id, uint64_t initial_timestamp, std::atomic<int>& progress_counter) {
    struct ActiveOrderRecord {
        uint32_t id;
        char side;
    };

    uint32_t tickerId = thread_id;
    std::string filename = Config::tickers[tickerId] + ".dat";
    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    std::vector<ActiveOrderRecord> active_orders;
    active_orders.reserve(num_instructions * 0.6);
    std::vector<ActiveOrderRecord> inactive_orders;
    inactive_orders.reserve(num_instructions * 0.3);

    std::mt19937 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count() + thread_id);
    std::uniform_int_distribution<int> qty_dist(Config::minGenQty, Config::maxGenQty);
    std::uniform_real_distribution<double> price_dist(Config::minGenPrice, Config::maxGenPrice);
    std::uniform_int_distribution<int> instruction_type_dist(1, 100);
    std::uniform_real_distribution<double> stale_dist(0.0, 1.0);

    uint32_t id_counter = initial_id;
    
    std::vector<char> buffer;
    buffer.reserve(16 * 1024 * 1024); // 16MB buffer

    const int progress_step = num_instructions / 10;

    for (int i = 0; i < num_instructions; ++i) {
        char line[256];
        int len = 0;

        int instruction_type_roll = instruction_type_dist(rng);
        bool force_add = active_orders.empty();

        uint64_t timestamp = initial_timestamp + i;

        if (!force_add && instruction_type_roll <= Config::cancelInstructionWeight) { // CANCEL
            bool generate_stale = Config::staleInstruction && !inactive_orders.empty() && stale_dist(rng) < Config::staleInstructionProbability;
            ActiveOrderRecord record_to_modify;

            if (generate_stale) {
                std::uniform_int_distribution<int> inactive_order_dist(0, inactive_orders.size() - 1);
                record_to_modify = inactive_orders[inactive_order_dist(rng)];
            } else {
                std::uniform_int_distribution<int> active_order_dist(0, active_orders.size() - 1);
                int order_idx = active_order_dist(rng);
                record_to_modify = active_orders[order_idx];
                inactive_orders.push_back(active_orders[order_idx]);
                std::swap(active_orders[order_idx], active_orders.back());
                active_orders.pop_back();
            }
            len = snprintf(line, sizeof(line), "%u;%s;%c;0.00;0;C;%lu\n",
                           record_to_modify.id, Config::tickers[tickerId].c_str(),
                           record_to_modify.side, timestamp);

        } else if (!force_add && instruction_type_roll <= (Config::cancelInstructionWeight + Config::editInstructionWeight)) { // EDIT
            bool generate_stale = Config::staleInstruction && !inactive_orders.empty() && stale_dist(rng) < Config::staleInstructionProbability;
            ActiveOrderRecord record_to_modify;

            if (generate_stale) {
                std::uniform_int_distribution<int> inactive_order_dist(0, inactive_orders.size() - 1);
                record_to_modify = inactive_orders[inactive_order_dist(rng)];
            } else {
                std::uniform_int_distribution<int> active_order_dist(0, active_orders.size() - 1);
                record_to_modify = active_orders[active_order_dist(rng)];
            }
            len = snprintf(line, sizeof(line), "%u;%s;%c;%.2f;%u;E;%lu\n",
                           record_to_modify.id, Config::tickers[tickerId].c_str(),
                           record_to_modify.side, price_dist(rng), qty_dist(rng) * 10, timestamp);

        } else { // ADD
            char side = (rng() % 2 == 0) ? 'B' : 'S';
            len = snprintf(line, sizeof(line), "%u;%s;%c;%.2f;%u;A;%lu\n",
                           id_counter, Config::tickers[tickerId].c_str(),
                           side, price_dist(rng), qty_dist(rng) * 10, timestamp);
            active_orders.push_back({id_counter, side});
            id_counter++;
        }
        
        buffer.insert(buffer.end(), line, line + len);

        if (buffer.size() >= 16 * 1024 * 1024) {
            outfile.write(buffer.data(), buffer.size());
            buffer.clear();
        }

        if (progress_step > 0 && (i % progress_step == 0)) {
            progress_counter++;
        }
    }

    if (!buffer.empty()) {
        outfile.write(buffer.data(), buffer.size());
    }
}

int main(int argc, char* argv[]) {
    auto start_time = std::chrono::high_resolution_clock::now();

    unsigned int num_threads = Config::tickers.size();
    std::vector<std::thread> threads;
    int instructions_per_ticker = Config::numInstructions / num_threads;
    std::atomic<int> progress_counter(0);

    for (unsigned int i = 0; i < num_threads; ++i) {
        uint32_t initial_id = Config::initialOrderId + (i * instructions_per_ticker * 2);
        uint64_t initial_timestamp = Config::initialTimestamp;
        threads.emplace_back(generator_thread_func, i, instructions_per_ticker, initial_id, initial_timestamp, std::ref(progress_counter));
    }

    // Progress reporting
    int percent_complete = 0;
    while(percent_complete < 100) {
        int current_progress = progress_counter.load();
        percent_complete = static_cast<int>((static_cast<double>(current_progress) / (num_threads * 10)) * 100);
        if (percent_complete > 100) percent_complete = 100;
        std::cout << "Generated " << percent_complete << "% of instructions..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_s = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();

    std::cout << "Generated " << Config::numInstructions << " instructions into per-ticker files in " << duration_s << " seconds." << std::endl;

    return 0;
}