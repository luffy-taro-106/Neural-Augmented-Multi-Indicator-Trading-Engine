#ifndef MACD_STRATEGY_H
#define MACD_STRATEGY_H

#include "data_types.h"

TradeResult run_macd_strategy(const std::vector<Candle>& candles, double profit_threshold = 0.01);

#endif // MACD_STRATEGY_H
