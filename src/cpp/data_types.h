#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <string>
#include <vector>

struct Candle {
    double open;
    double high;
    double low;
    double close;
    double volume;
};

struct TradeResult {
    double success_rate;
    double avg_return;
    int total_trades;
    std::vector<int> positions;
};

#endif // DATA_TYPES_H
