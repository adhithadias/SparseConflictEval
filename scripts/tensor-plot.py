import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import matplotlib
from enum import Enum

class FontSize(Enum):
    SMALL_SIZE = 8
    MEDIUM_SIZE = 12
    BIGGER_SIZE = 16
    
plt.rcParams.update({'font.size': FontSize.BIGGER_SIZE.value})

matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42

datafile = 'data/tensorcontract-1dout.csv'
savefile1 = 'images/tensorcontract-1dout.pdf'
savefile2 = 'images/tensorcontract-1dout-speedup.pdf'

# datafile = 'data/tensor-elwisemul.csv'
# savefile = 'images/tensor-elwisemul.pdf'

def convert_rows_to_string(row):
    print('row', row)
    if row / 1_000_000 > 1:
        return f"{row / 1_000_000:.1f}M"
    elif row / 1_000 > 1:
        return f"{row / 1_000:.1f}K"
    else:
        return f"{row}"

# Load data from CSV file
# Replace 'data.csv' with the actual path to your CSV file
df = pd.read_csv(datafile)
df.columns = df.columns.str.strip()
tensors = pd.read_csv('data/tensors.csv')
tensors.columns = tensors.columns.str.strip()
coo_sort = pd.read_csv('data/d3_coo_qsort.csv')
coo_sort.columns = coo_sort.columns.str.strip()

# remove .mtx from the matrix column
df['Tensor'] = df['tensor'].str.replace('.tns', '')

# Calculate the sum of Transpose and Taco for each matrix
df['Taco Transpose + Taco (ms)'] = df['transpose(ms)'] + df['taco(ms)']
df['COO Transpose + Taco (ms)'] = coo_sort['sort time all(ms)'] + df['taco(ms)']
df['COO Transpose Only + Taco (ms)'] = coo_sort['sort only(ms)'] + df['taco(ms)']

df['Density'] = tensors['Nnz'] / (tensors['D0'] * tensors['D1'] * tensors['D2'])

# Calculate speedup of Fused over Transpose + Taco
df['Speedup vs Taco'] = df['Taco Transpose + Taco (ms)'] / df['ours(ms)']
df['Speedup vs Trnsp Manual + Taco'] = df['COO Transpose + Taco (ms)'] / df['ours(ms)']
df['Speedup vs Sort Only + Taco'] = df['COO Transpose Only + Taco (ms)'] / df['ours(ms)']

df['D0'] = tensors['D0'].apply(lambda x: convert_rows_to_string(x))
df['D1'] = tensors['D1'].apply(lambda x: convert_rows_to_string(x))
df['D2'] = tensors['D2'].apply(lambda x: convert_rows_to_string(x))
df['Nnz'] = tensors['Nnz'].apply(lambda x: convert_rows_to_string(x))

print(df)

# Set up the figure with two subplots
fig, ax1 = plt.subplots(1, 1, figsize=(14, 7))
# fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(28, 7))

# Set the positions and width for the bars
group_spacing = 0.5
indices = np.arange(len(df)) * (group_spacing + 1)
bar_width = 0.25

# First subplot: Performance comparison (bar chart)
# Plot the bar for Fused (Ours)
fused_bars = ax1.bar(indices, df['ours(ms)'], bar_width, label='Ours')

# Plot the stacked bar for Transpose + Taco with Taco at the bottom
taco_bars = ax1.bar(indices + bar_width, df['taco(ms)'], bar_width, label='Taco (ms)', color='gray')
transpose_bars = ax1.bar(indices + bar_width, coo_sort['sort only(ms)'], bar_width, bottom=df['taco(ms)'], label='COO Sort Time Only')

# Plot the stacked bar for Taco Transpose + Taco with Taco at the bottom
taco_only_bars = ax1.bar(indices + 2 * bar_width, df['taco(ms)'], bar_width, color='gray')
taco_transpose_bars = ax1.bar(indices + 2 * bar_width, df['transpose(ms)'], bar_width, bottom=df['taco(ms)'], label='Taco Transpose')

# Set log scale for the first subplot
ax1.set_yscale('log')
ax1.set_ylabel('Time (ms in Log Scale)')

# Set custom x-axis labels
xtick_labels = [f"{tensor[0:10]}\n{tensor[10:]}" if len(tensor) > 10 else tensor for tensor in df['Tensor']]
ax1.set_xticks(indices + bar_width)
ax1.set_xticklabels(xtick_labels, rotation=0)

ax1.set_xlabel('Tensor')
# Move legend to the left side inside the plot area
ax1.legend(loc="upper left", ncol=2)
# ax1.set_title('Performance Comparison: Time (ms)')

plt.tight_layout()
plt.savefig(savefile1)

fig, ax2 = plt.subplots(1, 1, figsize=(14, 7))

# Second subplot: Speedup comparison (bar chart)
speedup_bar_width = 0.35
ax2.bar(indices, df['Speedup vs Sort Only + Taco'], speedup_bar_width, 
        label='Speedup vs. [COO Sort Only + Taco]', color='black', alpha=0.7)
ax2.bar(indices + speedup_bar_width, df['Speedup vs Taco'], speedup_bar_width, 
        label='Speedup vs. [Transpose + Taco]', color='blue', alpha=0.7)

# Set log scale for the second subplot
ax2.set_yscale('log')
ax2.set_ylabel('Speedup (Log Scale)')
ax2.set_xlabel('Tensor')

# Set custom x-axis labels for second subplot
ax2.set_xticks(indices + speedup_bar_width/2)
ax2.set_xticklabels(xtick_labels, rotation=0)

# Add a horizontal line at speedup = 1
ax2.axhline(y=1, color='red', linestyle='--', linewidth=2, label='Speedup = 1')

# Add legend for the second subplot
ax2.legend(loc="upper left")
# ax2.set_title('Speedup Comparison')
ax2.grid(True, alpha=0.3, axis='y')

# Display the plot
plt.tight_layout()
plt.savefig(savefile2)