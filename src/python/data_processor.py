import pandas as pd
from bindings import Candle, run_macd_strategy, run_rsi_strategy, run_supertrend_strategy # type: ignore

# Load CSV, skip metadata rows, assign correct headers
df = pd.read_csv("AAPL_training.csv", skiprows=2, header=None)
df.columns = ["Date", "Close", "High", "Low", "Open", "Volume"]

#test
df2 = pd.read_csv("AAPL_testing.csv", skiprows=2, header=None)
df2.columns = ["Date", "Close", "High", "Low", "Open", "Volume"]


# Convert relevant columns to float
df = df.astype({
    "Open": float,
    "High": float,
    "Low": float,
    "Close": float,
    "Volume": float
})

#test
df2 = df2.astype({
    "Open": float,
    "High": float,
    "Low": float,
    "Close": float,
    "Volume": float
})



# Convert to list of Candle objects
candles = [
    Candle(
        open=row["Open"],
        high=row["High"],
        low=row["Low"],
        close=row["Close"],
        volume=row["Volume"]
    )
    for _, row in df.iterrows()
]

#test
candles2 = [
    Candle(
        open=row["Open"],
        high=row["High"],
        low=row["Low"],
        close=row["Close"],
        volume=row["Volume"]
    )
    for _, row in df2.iterrows()
]

# Run MACD Strategy
macd_result = run_macd_strategy(candles)
print("\n🔹 MACD Strategy")
print("Success Rate:", macd_result.success_rate)
print("Per-Trade Return:", macd_result.avg_return)
print("Total Trades:", macd_result.total_trades)

# Run RSI Strategy
rsi_result = run_rsi_strategy(candles)
print("\n🔹 RSI Strategy")
print("Success Rate:", rsi_result.success_rate)
print("Per-Trade Return:", rsi_result.avg_return)
print("Total Trades:", rsi_result.total_trades)

# Run Supertrend Strategy
supertrend_result = run_supertrend_strategy(candles, atr_period=10, multiplier=3.0)
print("\n🔹 Supertrend Strategy")
print("Success Rate:", supertrend_result.success_rate)
print("Per-Trade Return:", supertrend_result.avg_return)
print("Total Trades:", supertrend_result.total_trades)

start_index = 26

positions_df = pd.DataFrame({
    "MACD": macd_result.positions[start_index:],
    "RSI": rsi_result.positions[start_index:],
    "Supertrend": supertrend_result.positions[start_index:]
})


price_df = df[["Close"]].copy()

returns_df = price_df.pct_change() * 100
returns_df = (returns_df > 0).astype(int)
returns_df = returns_df.iloc[start_index:].reset_index(drop=True)
returns_df= returns_df.shift(-1)
returns_df = returns_df.fillna(0)
returns_df.columns = ["Return"]

positions_df.to_excel("output.xlsx", index=False)
#test
macd_result2 = run_macd_strategy(candles2)
rsi_result2 = run_rsi_strategy(candles2)
supertrend_result2 = run_supertrend_strategy(candles2, atr_period=10, multiplier=3.0)

positions_df2 = pd.DataFrame({
    "MACD": macd_result2.positions[start_index:],
    "RSI": rsi_result2.positions[start_index:],
    "Supertrend": supertrend_result2.positions[start_index:]
})

price_df2 = df2[["Close"]].copy()

returns_df2 = price_df2.pct_change() * 100
returns_df2= (returns_df2 > 0).astype(int)
returns_df2 = returns_df2.iloc[start_index:].reset_index(drop=True)
returns_df2= returns_df2.shift(-1)
returns_df2 = returns_df2.fillna(0)
returns_df2.columns = ["Return"]