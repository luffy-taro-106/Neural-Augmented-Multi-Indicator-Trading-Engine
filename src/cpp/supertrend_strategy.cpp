#include "supertrend_strategy.h"
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

double calculate_atr(const vector<Candle> &candles, int current_index, int period)
{
    double atr = 0.0;
    for (int i = current_index - period + 1; i <= current_index; ++i)
    {
        double high = candles[i].high;
        double low = candles[i].low;
        double prev_close = candles[i - 1].close;
        double tr = max({high - low, fabs(high - prev_close), fabs(low - prev_close)});
        atr += tr;
    }
    return atr / period;
}

double calculate_volatility(const vector<Candle> &candles, int current_index, int vol_period)
{
    if (current_index < vol_period)
        return 1.0; // Allow trades initially

    vector<double> returns;
    for (int i = current_index - vol_period + 1; i <= current_index; ++i)
    {
        double ret = (candles[i].close - candles[i - 1].close) / candles[i - 1].close;
        returns.push_back(ret);
    }

    double mean = 0.0;
    for (double r : returns)
        mean += r;
    mean /= returns.size();

    double variance = 0.0;
    for (double r : returns)
        variance += (r - mean) * (r - mean);
    variance /= returns.size();

    return sqrt(variance); // This is standard deviation
}

TradeResult run_supertrend_strategy(const vector<Candle> &candles, int period, double multiplier, double profit_threshold)
{
    vector<double> closes;
    for (const auto &candle : candles)
        closes.push_back(candle.close);

    enum Position { NONE, LONG, SHORT };
    Position state = NONE;

    vector<double> final_upper_band(candles.size(), 0.0);
    vector<double> final_lower_band(candles.size(), 0.0);
    vector<bool> in_uptrend(candles.size(), true);
    vector<int> supertrend_positions(closes.size(), 0);

    double entry_price = 0.0;
    int entry_index = -1;

    int profitable_trades = 0;
    int total_trades = 0;
    double total_return = 0.0;

    bool prev_uptrend = true;
    const int volatility_period = 10;

    for (size_t i = period; i < candles.size(); ++i)
    {
        double volatility = calculate_volatility(candles, i, volatility_period);
        if (volatility < 0.01) {
            supertrend_positions[i] = 0;
            continue; // Skip trading due to low volatility
        }

        double atr = calculate_atr(candles, i, period);
        double hl2 = (candles[i].high + candles[i].low) / 2.0;

        double upper_band = hl2 + multiplier * atr;
        double lower_band = hl2 - multiplier * atr;

        if (i > 0)
        {
            if (candles[i].close > final_upper_band[i - 1])
                in_uptrend[i] = true;
            else if (candles[i].close < final_lower_band[i - 1])
                in_uptrend[i] = false;
            else
                in_uptrend[i] = in_uptrend[i - 1];

            if (in_uptrend[i])
                lower_band = max(lower_band, final_lower_band[i - 1]);
            else
                upper_band = min(upper_band, final_upper_band[i - 1]);
        }

        final_upper_band[i] = upper_band;
        final_lower_band[i] = lower_band;

        // SIGNAL ENCODING
        if (!prev_uptrend && in_uptrend[i])
        {
            supertrend_positions[i] = 1;  // Buy
        }
        else if (prev_uptrend && !in_uptrend[i])
        {
            supertrend_positions[i] = -1; // Exit long
        }
        else if (!prev_uptrend && !in_uptrend[i])
        {
            supertrend_positions[i] = -2; // Short
        }
        else if (prev_uptrend && in_uptrend[i])
        {
            supertrend_positions[i] = 0;  // Holding
        }

        if (!prev_uptrend && in_uptrend[i])
        {
            // Trend switches from down to up
            if (state == SHORT)
            {
                double exit_price = candles[i].close;
                double ret = (entry_price - exit_price) / entry_price;
                total_return += ret;
                if (ret > profit_threshold)
                    profitable_trades++;
                total_trades++;
            }

            // Open long
            state = LONG;
            entry_price = candles[i].close;
            entry_index = i;
        }
        else if (prev_uptrend && !in_uptrend[i])
        {
            // Trend switches from up to down
            if (state == LONG)
            {
                double exit_price = candles[i].close;
                double ret = (exit_price - entry_price) / entry_price;
                total_return += ret;
                if (ret > profit_threshold)
                    profitable_trades++;
                total_trades++;
            }

            // Open short
            state = SHORT;
            entry_price = candles[i].close;
            entry_index = i;
        }

        prev_uptrend = in_uptrend[i];
    }

    // Final trade exit
    if (state != NONE)
    {
        double final_price = candles.back().close;
        double ret = state == LONG
                         ? (final_price - entry_price) / entry_price
                         : (entry_price - final_price) / entry_price;

        total_return += ret;
        if (ret > profit_threshold)
            profitable_trades++;
        total_trades++;
    }

    double success_rate = total_trades > 0 ? (double)profitable_trades / total_trades * 100 : 0;
    double avg_return = total_trades > 0 ? (total_return / total_trades) * 100 : 0;

    return {success_rate, avg_return, total_trades, supertrend_positions};
}
