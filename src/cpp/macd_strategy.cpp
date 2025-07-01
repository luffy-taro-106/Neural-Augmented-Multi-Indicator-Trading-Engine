#include "macd_strategy.h"
#include <vector>
#include <cmath>

using namespace std;

double calculate_ema(const vector<double> &data, int index, int period)
{
    if (index < period)
        return data[index];

    double k = 2.0 / (period + 1);
    double ema = data[index - period];
    for (int i = index - period + 1; i <= index; ++i)
        ema = data[i] * k + ema * (1 - k);
    return ema;
}

double calculate_stddev(const vector<double> &data, int start, int end)
{
    double mean = 0.0;
    for (int i = start; i < end; ++i)
        mean += data[i];
    mean /= (end - start);

    double variance = 0.0;
    for (int i = start; i < end; ++i)
        variance += (data[i] - mean) * (data[i] - mean);
    return sqrt(variance / (end - start));
}

TradeResult run_macd_strategy(const vector<Candle> &candles, double profit_threshold)
{
    vector<double> closes;
    for (const auto &candle : candles)
        closes.push_back(candle.close);

    vector<int> macd_positions(closes.size(), 0);
    vector<double> macd_line(closes.size(), 0.0);
    vector<double> signal_line(closes.size(), 0.0);
    vector<double> histogram(closes.size(), 0.0);
    vector<double> ema50(closes.size(), 0.0);

    // MACD & signal line
    for (size_t i = 26; i < closes.size(); ++i)
    {
        double ema12 = calculate_ema(closes, i, 12);
        double ema26 = calculate_ema(closes, i, 26);
        macd_line[i] = ema12 - ema26;
    }

    for (size_t i = 35; i < closes.size(); ++i)
        signal_line[i] = calculate_ema(macd_line, i, 9);

    for (size_t i = 35; i < closes.size(); ++i)
        histogram[i] = macd_line[i] - signal_line[i];

    for (size_t i = 50; i < closes.size(); ++i)
        ema50[i] = calculate_ema(closes, i, 50);

    int profitable_trades = 0;
    double total_return = 0.0;
    int total_trades = 0;

    double entry_price = 0.0;
    int entry_index = -1;
    enum Position { NONE, LONG, SHORT } state = NONE;

    const int hist_window = 10;
    const double min_hist = 0.005;         // absolute value threshold
    const double std_multiplier = 1.0;     // dynamic filter strength
    const int min_trade_duration = 3;      // to avoid scalp noise

    for (size_t i = 60; i < closes.size(); ++i)
    {
        double hist = histogram[i];
        double dyn_threshold = calculate_stddev(histogram, i - hist_window, i) * std_multiplier;

        bool trend_up = closes[i] > ema50[i];
        bool trend_down = closes[i] < ema50[i];

        // Entry logic
        if (state == NONE)
        {
            if (hist > dyn_threshold && hist > min_hist && trend_up)
            {
                macd_positions[i] = 1;
                entry_price = closes[i];
                entry_index = i;
                state = LONG;
            }
            else if (hist < -dyn_threshold && hist < -min_hist && trend_down)
            {
                macd_positions[i] = -2;
                entry_price = closes[i];
                entry_index = i;
                state = SHORT;
            }
        }

        // Exit logic
        else if (state == LONG)
        {
            if ((hist < dyn_threshold || !trend_up) && i - entry_index >= min_trade_duration)
            {
                macd_positions[i] = -1;
                double ret = (closes[i] - entry_price) / entry_price;
                total_return += ret;
                if (ret > profit_threshold) profitable_trades++;
                total_trades++;
                state = NONE;
            }
        }
        else if (state == SHORT)
        {
            if ((hist > -dyn_threshold || !trend_down) && i - entry_index >= min_trade_duration)
            {
                macd_positions[i] = 2;
                double ret = (entry_price - closes[i]) / entry_price;
                total_return += ret;
                if (ret > profit_threshold) profitable_trades++;
                total_trades++;
                state = NONE;
            }
        }
    }

    // Force close
    if (state != NONE)
    {
        double final_price = closes.back();
        double ret = state == LONG
                         ? (final_price - entry_price) / entry_price
                         : (entry_price - final_price) / entry_price;

        total_return += ret;
        if (ret > profit_threshold) profitable_trades++;
        total_trades++;
    }

    double success_rate = total_trades > 0 ? (double)profitable_trades / total_trades * 100 : 0;
    double avg_return = total_trades > 0 ? (total_return / total_trades) * 100 : 0;

    return {success_rate, avg_return, total_trades, macd_positions};
}
