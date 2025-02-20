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

chou_etal = "data/chou-csr-to-csc.csv"

datafile = 'data/elementwise-mul.csv'
savefile = 'images/elementwise-mul.pdf'

# datafile = 'data/dotprod-denseout.csv'
# savefile = 'images/dotprod-denseout.pdf'

# Load data from CSV file
# Replace 'data.csv' with the actual path to your CSV file
df = pd.read_csv(datafile)
df2 = pd.read_csv(chou_etal)
matrices = pd.read_csv('data/matrices.csv')

# Strip any whitespace from headers
df.columns = df.columns.str.strip()
df2.columns = df2.columns.str.strip()
matrices.columns = matrices.columns.str.strip()
df['Chou Transpose (ms)']= df2['Chou Transpose (ms)']

# remove .mtx from the matrix column
df['Matrix'] = df['Matrix'].str.replace('.mtx', '')

# Calculate the sum of Transpose and Taco for each matrix
df['Transpose + Taco (ms)'] = df['Transpose (ms)'] + df['Taco (ms)']
df['Chou Transpose + Taco (ms)'] = df['Chou Transpose (ms)'] + df['Taco (ms)']
df['Taco Transpose + Taco (ms)'] = df['Taco Transpose (ms)'] + df['Taco (ms)']

df['Density'] = matrices['Nnz'] / (matrices['Rows'] * matrices['Columns'])

# Calculate speedup of Fused over Transpose + Taco
df['Speedup'] = df['Transpose + Taco (ms)'] / df['Fused(Ours) (ms)']
df['Speedup Chou'] = df['Chou Transpose + Taco (ms)'] / df['Fused(Ours) (ms)']

print(df)

# Set up the figure and axis
fig, ax1 = plt.subplots(figsize=(14, 8))

# Set the positions and width for the bars
group_spacing = 0.5
indices = np.arange(len(df)) * (group_spacing + 1)
bar_width = 0.25

# Plot the bar for Fused (Ours)
fused_bars = ax1.bar(indices, df['Fused(Ours) (ms)'], bar_width, label='Fused (Ours) (ms)')

# Plot the stacked bar for Transpose + Taco with Taco at the bottom
taco_bars = ax1.bar(indices + bar_width, df['Taco (ms)'], bar_width, label='Taco Execution (ms)', color='gray')
transpose_bars = ax1.bar(indices + bar_width, df['Transpose (ms)'], bar_width, bottom=df['Taco (ms)'], label='Scipy Transpose (ms)')

# Plot the stacked bar for Taco Transpose + Taco with Taco at the bottom
taco_only_bars = ax1.bar(indices + 2 * bar_width, df['Taco (ms)'], bar_width, color='gray')
taco_transpose_bars = ax1.bar(indices + 2 * bar_width, df['Chou Transpose (ms)'], bar_width, bottom=df['Taco (ms)'], label='Taco (Chou et al)\nTranspose (ms)')

# Set log scale for the primary y-axis
ax1.set_yscale('log')
ax1.set_ylabel('Time (ms) - Log Scale')

def convert_rows_to_string(row):
    if row / 1_000_000 > 1:
        return f"{row / 1_000_000:.1f}M"
    elif row / 1_000 > 1:
        return f"{row / 1_000:.1f}K"
    else:
        return f"{row}"

# Set custom x-axis labels with Matrix and Nnz values
# xtick_labels = [f"{matrix}\nNnz: {convert_rows_to_string(nnz)}, N: {convert_rows_to_string(n)}\nDensity: {'%.2E' % density}" for matrix, nnz, density, n in zip(df['Matrix'], df['Nnz'], df['Density'], df['Rows'])]
xtick_labels = [f"{matrix[0:10]}" for matrix in df['Matrix']]
ax1.set_xticks(indices + bar_width)
ax1.set_xticklabels(xtick_labels, rotation=30, ha="right")

# Set labels and title for primary axis
ax1.set_xlabel('Matrix and Non-Zero Elements')
# ax1.set_title('Performance Comparison: Transpose + Taco vs. Fused (Ours) vs. Taco Transpose + Taco with Speedup')
ax1.legend(loc="upper center", bbox_to_anchor=(0.5, 1.15), ncol=4)

# Add a legend for the primary axis
# ax1.legend(loc="upper center", bbox_to_anchor=(1.05, 1), borderaxespad=0.)

# Create a secondary y-axis for the speedup
ax2 = ax1.twinx()
ax2.plot(indices + bar_width, df['Speedup Chou'], color='none', marker='o', label='Speedup (Fused (Ours) vs.\n        [Taco (Chou et al) Transpose + Taco)]')
ax2.plot(indices + bar_width, df['Speedup'], color='black', marker='o', label='Speedup (Fused (Ours) vs. [Scipy Transpose + Taco)]')

ax2.set_ylabel('Speedup \n(Fused (Ours) / [Transpose + Taco])', color='black')
ax2.tick_params(axis='y', labelcolor='black')

# Add a broken horizontal line at speedup = 1
ax2.axhline(y=1, color='gray', linestyle='--', linewidth=1, label='Speedup = 1')

# Add legend for the secondary axis
legend = ax2.legend(loc="upper left")
text = legend.get_texts()[0]

# Set the color of the first label to white
text.set_color('white')

# Display the plot
plt.tight_layout()
plt.savefig(savefile)
