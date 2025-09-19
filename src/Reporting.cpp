// ----------------------------------------------------------------------------- //
//                                                                               //
//  Order Book Simulator                                                         //
//  Copyright (c) 2025 Flavio Milinanni. All Rights Reserved.                    //
//                                                                               //
// ----------------------------------------------------------------------------- //

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <numeric>

#include "../include/Reporting.h"
#include "../include/Configuration.h"

void print_table(const std::vector<TickerResult>& results) {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n+----------+------------+----------+----------+----------+----------------+----------------+----------+\n";
    std::cout << "|  TICKER  |   TOTAL    |   ADDs   | CANCELs  |  EDITs   | FAILED CANCELS |  FAILED EDITS  | TIME(ms) |\n";
    std::cout << "+----------+------------+----------+----------+----------+----------------+----------------+----------+\n";

    long long total_instructions = 0;
    long long total_adds = 0;
    long long total_cancels = 0;
    long long total_edits = 0;
    long long total_failed_cancels = 0;
    long long total_failed_edits = 0;
    double max_time_ms = 0.0;

    for (const auto& r : results) {
        std::cout << "| " << std::setw(8) << std::left << r.name << " | "
                  << std::setw(10) << std::right << r.instruction_count << " | "
                  << std::setw(8) << r.add_count << " | "
                  << std::setw(8) << r.cancel_count << " | "
                  << std::setw(8) << r.edit_count << " | "
                  << std::setw(14) << r.failed_cancels << " | "
                  << std::setw(14) << r.failed_edits << " | "
                  << std::setw(8) << r.time_ms << " |\n";
        total_instructions += r.instruction_count;
        total_adds += r.add_count;
        total_cancels += r.cancel_count;
        total_edits += r.edit_count;
        total_failed_cancels += r.failed_cancels;
        total_failed_edits += r.failed_edits;
        if (r.time_ms > max_time_ms) {
            max_time_ms = r.time_ms;
        }
    }

    std::cout << "+----------+------------+----------+----------+----------+----------------+----------------+----------+\n";
    std::cout << "|  TOTAL   | " << std::setw(10) << std::right << total_instructions << " | "
              << std::setw(8) << total_adds << " | "
              << std::setw(8) << total_cancels << " | "
              << std::setw(8) << total_edits << " | "
              << std::setw(14) << total_failed_cancels << " | "
              << std::setw(14) << total_failed_edits << " | "
              << std::setw(8) << max_time_ms << " |\n";
    std::cout << "+----------+------------+----------+----------+----------+----------------+----------------+----------+\n";

    double instructions_per_second = (total_instructions > 0 && max_time_ms > 0) ? (total_instructions / (max_time_ms / 1000.0)) : 0;
    double latency_ns = (total_instructions > 0) ? (max_time_ms * 1000000.0 / total_instructions) : 0;

    std::cout << "\nSummary:\n";
    std::cout << "  Instructions/sec: " << instructions_per_second / 1e6 << " M/s\n";
    std::cout << "  Avg. Latency/Inst: " << latency_ns << " ns\n";
}