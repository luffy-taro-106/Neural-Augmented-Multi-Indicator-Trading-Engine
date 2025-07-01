#ifndef RSI_STRATEGY_H
#define RSI_STRATEGY_H

#include "data_types.h"
TradeResult run_rsi_strategy(const std::vector<Candle> &candles, double profit_threshold);

#endif // RSI_STRATEGY_H
