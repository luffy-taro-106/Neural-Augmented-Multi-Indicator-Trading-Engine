import torch  # type: ignore
import numpy as np
import pandas as pd
from model import SignalNN
from data_processor import positions_df2, df2

# Use raw close prices for real return calculation
close_prices = df2["Close"].values

# Prepare input features
X_test = positions_df2[["MACD", "RSI", "Supertrend"]].values.astype(np.float32)

# Convert to PyTorch tensor
X_tensor = torch.tensor(X_test)

# Load model
model = SignalNN()
model.load_state_dict(torch.load("nn_model_weights.pth"))
model.eval()

# Predict
with torch.no_grad():
    outputs = model(X_tensor).numpy().flatten()
    preds = (outputs > 0.5).astype(int)

# Real per-trade return logic
in_position = False
entry_price = 0
total_return = 0
total_trades = 0
profitable_trades = 0
profit_threshold = 0.0

for i in range(len(preds)):
    if not in_position and preds[i] == 1:
        in_position = True
        entry_price = close_prices[i + 26]  # +26 due to start_index shift
    elif in_position and preds[i] == 0:
        exit_price = close_prices[i + 26]
        ret = (exit_price - entry_price) / entry_price
        total_return += ret
        if ret > profit_threshold:
            profitable_trades += 1
        total_trades += 1
        in_position = False

# If still holding at end
if in_position:
    exit_price = close_prices[-1]
    ret = (exit_price - entry_price) / entry_price
    total_return += ret
    if ret > profit_threshold:
        profitable_trades += 1
    total_trades += 1

success_rate = (profitable_trades / total_trades * 100) if total_trades > 0 else 0
avg_return = (total_return / total_trades * 100) if total_trades > 0 else 0

# Print results
print("\n📊 Neural Network Strategy Results")
print(f"✅ Success Rate: {success_rate:.2f}%")
print(f"📈 Per-Trade Return: {avg_return:.2f}")
print(f"📉 Total Trades Executed: {total_trades}")
