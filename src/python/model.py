
import torch
import torch.nn as nn
import torch.nn.functional as F

class SignalNN(nn.Module):
    def __init__(self):
        super(SignalNN, self).__init__()
        self.fc1 = nn.Linear(3, 16)  # 3 input features
        self.fc2 = nn.Linear(16, 8)
        self.fc3 = nn.Linear(8, 1)   # Output: single value (0 or 1)

    def forward(self, x):
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = torch.sigmoid(self.fc3(x))
        return x