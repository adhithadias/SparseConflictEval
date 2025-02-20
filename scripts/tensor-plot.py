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
savefile = 'images/tensorcontract-1dout.pdf'

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
coo_sort = pd.read_csv('data/d3_coo_sort.csv')
coo_sort.columns = coo_sort.columns.str.strip()

# remove .mtx from the matrix column
df['Tensor'] = df['Tensor'].str.replace('.tns', '')

# Calculate the sum of Transpose and Taco for each matrix
df['Taco Transpose + Taco (ms)'] = df['Taco Transpose (ms)'] + df['Taco (ms)']
df['COO Transpose + Taco (ms)'] = coo_sort['COO Transpose (ms)'] + df['Taco (ms)']
df['COO Transpose Only + Taco (ms)'] = coo_sort['COO Sort Only (ms)'] + df['Taco (ms)']

df['Density'] = tensors['Nnz'] / (tensors['D0'] * tensors['D1'] * tensors['D2'])

# Calculate speedup of Fused over Transpose + Taco
df['Speedup vs Taco'] = df['Taco Transpose + Taco (ms)'] / df['Fused(Ours) (ms)']
df['Speedup vs Trnsp Manual + Taco'] = df['COO Transpose + Taco (ms)'] / df['Fused(Ours) (ms)']
df['Speedup vs Sort Only + Taco'] = df['COO Transpose Only + Taco (ms)'] / df['Fused(Ours) (ms)']

df['D0'] = tensors['D0'].apply(lambda x: convert_rows_to_string(x))
df['D1'] = tensors['D1'].apply(lambda x: convert_rows_to_string(x))
df['D2'] = tensors['D2'].apply(lambda x: convert_rows_to_string(x))
df['Nnz'] = tensors['Nnz'].apply(lambda x: convert_rows_to_string(x))

print(df)

# exit(0)

# Set up the figure and axis
fig, ax1 = plt.subplots(figsize=(14, 8))

# Set the positions and width for the bars
group_spacing = 0.5
indices = np.arange(len(df)) * (group_spacing + 1)
bar_width = 0.25

# Plot the bar for Fused (Ours)
fused_bars = ax1.bar(indices, df['Fused(Ours) (ms)'], bar_width, label='Fused (Ours) (ms)')

# Plot the stacked bar for Transpose + Taco with Taco at the bottom
taco_bars = ax1.bar(indices + bar_width, df['Taco (ms)'], bar_width, label='Taco (ms)', color='gray')
transpose_bars = ax1.bar(indices + bar_width, coo_sort['COO Sort Only (ms)'], bar_width, bottom=df['Taco (ms)'], label='COO Sort Time Only (ms)')

# Plot the stacked bar for Taco Transpose + Taco with Taco at the bottom
taco_only_bars = ax1.bar(indices + 2 * bar_width, df['Taco (ms)'], bar_width, color='gray')
taco_transpose_bars = ax1.bar(indices + 2 * bar_width, df['Taco Transpose (ms)'], bar_width, bottom=df['Taco (ms)'], label='Taco Transpose (ms)')

# Set log scale for the primary y-axis
ax1.set_yscale('log')
ax1.set_ylabel('Time (ms) - Log Scale')

# Set custom x-axis labels with Matrix and Nnz values
# xtick_labels = [f"{tensor}\nNnz: {nnz}, N: {n}\nDensity: {'%.2E' % density}" for tensor, nnz, density, n in zip(df['Tensor'], df['Nnz'], df['Density'], df['D0'])]
xtick_labels = [f"{tensor[0:10]}" for tensor in df['Tensor']]
ax1.set_xticks(indices + bar_width/2)
ax1.set_xticklabels(xtick_labels, rotation=0, ha="right")

# Set labels and title for primary axis
ax1.set_xlabel('Tensor')
# ax1.set_title('Performance Comparison: Transpose + Taco vs. Fused (Ours) vs. Taco Transpose + Taco with Speedup')

# Add a legend for the primary axis
# ax1.legend(loc="upper left", bbox_to_anchor=(1.05, 1), borderaxespad=0.)
# ax1.legend(loc="upper left")
ax1.legend(loc="upper center", bbox_to_anchor=(0.5, 1.15), ncol=4)

# Create a secondary y-axis for the speedup
ax2 = ax1.twinx()
ax2.plot(indices + bar_width, df['Speedup vs Sort Only + Taco'], color='black', marker='o', label='Speedup (Fused vs. [COO Sort Only + Taco])')
ax2.plot(indices + bar_width, df['Speedup vs Taco'], color='blue', marker='o', label='Speedup (Fused vs. [Transpose + Taco])')
ax2.set_ylabel('Speedup (Fused / Transpose + Taco)', color='black')
ax2.tick_params(axis='y', labelcolor='black')

# Add a broken horizontal line at speedup = 1
ax2.axhline(y=1, color='gray', linestyle='--', linewidth=1, label='Speedup = 1')

# Add legend for the secondary axis
ax2.legend(loc="upper left")

# Display the plot
plt.tight_layout()
plt.savefig(savefile)
