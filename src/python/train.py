# train.py
import torch # type: ignore
import torch.nn as nn # type: ignore
import torch.optim as optim # type: ignore
from model import SignalNN
from data_processor import positions_df, returns_df
import numpy as np

# Prepare the data
X = positions_df[["MACD", "RSI", "Supertrend"]].values.astype(np.float32)
y = (returns_df["Return"] > 0).astype(int).values.astype(np.float32).reshape(-1, 1)

# Create DataLoader
batch_size = 32
dataset = torch.utils.data.TensorDataset(torch.tensor(X), torch.tensor(y))
dataloader = torch.utils.data.DataLoader(dataset, batch_size=batch_size, shuffle=True)

# Initialize model, loss, and optimizer
model = SignalNN()
criterion = nn.BCELoss()
optimizer = optim.Adam(model.parameters(), lr=0.001)

# Training loop
epochs = 50
for epoch in range(epochs):
    total_loss = 0
    for xb, yb in dataloader:
        optimizer.zero_grad()
        preds = model(xb)
        loss = criterion(preds, yb)
        loss.backward()
        optimizer.step()
        total_loss += loss.item()
    print(f"Epoch {epoch+1}/{epochs}, Loss: {total_loss:.4f}")

# Save model weights
torch.save(model.state_dict(), "models/nn_model_weights.pth")
