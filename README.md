🔹 MACD Strategy
The MACD (Moving Average Convergence Divergence) strategy detects trend shifts using the difference between fast (12 EMA) and slow (26 EMA) exponential moving averages. A signal line (9 EMA of the MACD line) helps confirm entries.

Entry Conditions:

Long Entry: When the MACD histogram is strongly positive (above a dynamic volatility threshold), and price is above the 50 EMA (uptrend).

Short Entry: When the histogram is strongly negative and price is below the 50 EMA (downtrend).

Exit Conditions:

Exit after minimum 3 days if the histogram weakens or the trend condition breaks.

Filters & Rules:

Volatility filter using standard deviation of histogram over last 10 days.

Ignores trades in low-volatility environments to avoid noise.

Only considers trades valid if the return exceeds a given profit threshold.

🔹 RSI Strategy
The RSI (Relative Strength Index) strategy identifies overbought and oversold conditions to time long and short trades.

Entry Conditions:

Long Entry: RSI > 60 and recent price gain > 5% indicates momentum and strength.

Short Entry: RSI < 40 and recent price drop > 5% signals weakness.

Exit Conditions:

Exit Long: RSI drops below 60.

Exit Short: RSI rises above 40.

Filters & Rules:

Entry signals are only triggered once per cycle to avoid duplicates.

Includes a 1% price movement filter for meaningful volatility.

Strategy measures profitability using actual price-based return over trades, not just signals.

🔹 Supertrend Strategy
The Supertrend strategy identifies trend reversals using volatility-adjusted bands derived from ATR (Average True Range).

Logic Overview:

Upper and Lower Bands are computed using a price average (HL2) and a volatility multiplier.

A switch from below to above the band indicates an uptrend (Buy).

A switch from above to below indicates a downtrend (Short).

Entry & Exit:

Long Entry: Price crosses above upper band.

Short Entry: Price crosses below lower band.

Existing trades are exited when the trend reverses.

Filter:

A volatility threshold (1%) is used to avoid trades in flat markets.

Neural Network Model
trade signals based on the outputs of the three technical indicators: MACD, RSI, and Supertrend.

Architecture:

Input Layer: 3 neurons (one for each indicator signal)

Hidden Layer 1: 16 neurons with ReLU activation

Hidden Layer 2: 8 neurons with ReLU activation

Output Layer: 1 neuron with Sigmoid activation (predicts probability of a profitable trade)


Output:
A value between 0 and 1, interpreted as the probability of a successful trade. A threshold (e.g. 0.5) is applied to make a binary decision (Buy or No Trade).

Training Methodology:

Loss Function: Binary Cross Entropy

Optimizer: Adam

Features: Indicator signals from MACD, RSI, and Supertrend strategies

Labels: Binary target (1 = profitable trade, 0 = loss)

Evaluation Metrics: Success rate, average return, and total trades


Strategy Performance Summary
◆ MACD Strategy
Success Rate: 34.86%

Per-Trade Return: 0.2154%

Total Trades: 218

◆ RSI Strategy
Success Rate: 46.43%

Per-Trade Return: 2.1320%

Total Trades: 28

◆ Supertrend Strategy
Success Rate: 45.31%

Per-Trade Return: 4.9059%

Total Trades: 64

◆ Neural Network Strategy Results
Success Rate: 57.30%

Per-Trade Return: 0.95

Total Trades Executed: 89

Instructions - in command promt run 

mkdir build
cd build
cmake ..
cmake --build . --config Release

then in go to build -> release -> copy bindings.dll file ->
paste it in same folder as data_processor.py (python folder) -> rename it as bindings.pyd