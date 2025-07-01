#ifndef SUPERTREND_STRATEGY_H
#define SUPERTREND_STRATEGY_H

#include "data_types.h"

TradeResult run_supertrend_strategy(const std::vector<Candle> &candles, int period, double multiplier, double profit_threshold);

#endif // SUPERTREND_STRATEGY_H
