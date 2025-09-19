#ifndef TICKER_RESULT_INCLUDED
#define TICKER_RESULT_INCLUDED

#include <string>

struct TickerResult {
    std::string name;
    long long instruction_count = 0;
    long long add_count = 0;
    long long cancel_count = 0;
    long long edit_count = 0;
    long long failed_cancels = 0;
    long long failed_edits = 0;
    double time_ms = 0.0;
};

#endif // !TICKER_RESULT_INCLUDED
