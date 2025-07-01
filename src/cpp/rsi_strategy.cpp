#include "rsi_strategy.h"
#include <vector>
#include <cmath>

using namespace std;

double calculate_rsi(const vector<double> &closes, int current_index, int period = 14)
{
    if (current_index < period)
        return 50.0; // not enough data

    double gain = 0.0, loss = 0.0;

    for (int i = current_index - period + 1; i <= current_index; ++i)
    {
        double change = closes[i] - closes[i - 1];
        if (change > 0)
            gain += change;
        else
            loss -= change;
    }

    double rs = gain / loss;
    return 100.0 - (100.0 / (1.0 + rs));
}

TradeResult run_rsi_strategy(const vector<Candle> &candles, double profit_threshold)
{
    vector<double> closes;
    for (const auto &candle : candles)
        closes.push_back(candle.close);

    vector<int> rsi_positions(closes.size(), 0);

    int profitable_trades = 0;
    double total_return = 0.0;
    int total_trades = 0;

    bool was_above_60 = false;
    bool was_below_40 = false;

    double entry_price = 0.0;
    int entry_index = -1;
    enum Position
    {
        NONE,
        LONG,
        SHORT
    } state = NONE;

    for (size_t i = 15; i < closes.size(); ++i)
    {
        double rsi = calculate_rsi(closes, i);
        double price_change = (closes[i] - closes[i - 1]) / closes[i - 1];

        // Signal encoding with 1% movement filter
        if (rsi > 60 && price_change > 0.05)
        {
            if (!was_above_60)
            {
                rsi_positions[i] = 1; // Buy signal
                was_above_60 = true;
            }
            else
            {
                rsi_positions[i] = 0;
            }
            was_below_40 = false;
        }
        else if (was_above_60 && rsi < 60)
        {
            rsi_positions[i] = -1; // Exit long
            was_above_60 = false;
        }
        else if (rsi < 40 && price_change < -0.05)
        {
            if (!was_below_40)
            {
                rsi_positions[i] = -2; // Short signal
                was_below_40 = true;
            }
            else
            {
                rsi_positions[i] = 0;
            }
            was_above_60 = false;
        }
        else if (was_below_40 && rsi > 40)
        {
            rsi_positions[i] = 2; // Exit short
            was_below_40 = false;
        }
        else
        {
            rsi_positions[i] = 0;
        }

        // Trading logic with updated RSI thresholds
        if (state == NONE)
        {
            if (rsi > 60 && price_change > 0.05)
            {
                state = LONG;
                entry_price = closes[i];
                entry_index = i;
            }
            else if (rsi < 40 && price_change < -0.05)
            {
                state = SHORT;
                entry_price = closes[i];
                entry_index = i;
            }
        }
        else if (state == LONG && rsi < 60)
        {
            double exit_price = closes[i];
            double ret = (exit_price - entry_price) / entry_price;
            total_return += ret;
            if (ret > profit_threshold)
                profitable_trades++;
            total_trades++;
            state = NONE;
        }
        else if (state == SHORT && rsi > 40)
        {
            double exit_price = closes[i];
            double ret = (entry_price - exit_price) / entry_price;
            total_return += ret;
            if (ret > profit_threshold)
                profitable_trades++;
            total_trades++;
            state = NONE;
        }
    }

    // Force-close if needed
    if (state != NONE)
    {
        double final_price = closes.back();
        double ret = 0.0;
        if (state == LONG)
            ret = (final_price - entry_price) / entry_price;
        else if (state == SHORT)
            ret = (entry_price - final_price) / entry_price;

        total_return += ret;
        if (ret > profit_threshold)
            profitable_trades++;
        total_trades++;
    }

    double success_rate = total_trades > 0 ? (double)profitable_trades / total_trades * 100 : 0;
    double avg_return = total_trades > 0 ? (total_return / total_trades) * 100 : 0;

    return {success_rate, avg_return, total_trades, rsi_positions};
}
