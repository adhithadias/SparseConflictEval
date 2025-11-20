import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import matplotlib
from enum import Enum
import sys
import os


class FontSize(Enum):
    SMALL_SIZE = 8
    MEDIUM_SIZE = 12
    BIGGER_SIZE = 16


plt.rcParams.update({"font.size": FontSize.BIGGER_SIZE.value})

# # set font type to linux libertine and font to biolinum
# plt.rcParams['font.family'] = 'Linux Libertine'
# plt.rcParams['font.serif'] = 'Linux Biolinum'

matplotlib.rcParams["pdf.fonttype"] = 42
matplotlib.rcParams["ps.fonttype"] = 42

chou_etal = "data/chou-csr-to-csc.csv"
scipy_transpose = "data/scipy-csr-to-csc.csv"

# get which matrix to plot from command line
matrix_type = 0

# get matrix_type from command line
matrix_type = int(sys.argv[1])
print(f"Matrix type: {matrix_type}")

output_dir = f"images/{sys.argv[2]}/" if len(sys.argv) > 2 else "images/"
print(f"Output directory: {output_dir}")

# create output directory if it doesn't exist
if not os.path.exists(output_dir):
    os.makedirs(output_dir)


if matrix_type == 0:
    datafile = "data/elementwise-mul-with.csv"
    savefile1 = f"{output_dir}elementwise-mul-with.pdf"
    savefile2 = f"{output_dir}elementwise-mul-with-speedup.pdf"
elif matrix_type == 1:
    datafile = "data/dotprod-dense.csv"
    savefile1 = f"{output_dir}dotprod-dense.pdf"
    savefile2 = f"{output_dir}dotprod-dense-speedup.pdf"
else:
    datafile = "data/elementwise-mul-without.csv"
    savefile1 = f"{output_dir}elementwise-mul-without.pdf"
    savefile2 = f"{output_dir}elementwise-mul-without-speedup.pdf"


def convert_rows_to_string(row):
    if row / 1_000_000 > 1:
        return f"{row / 1_000_000:.1f}M"
    elif row / 1_000 > 1:
        return f"{row / 1_000:.1f}K"
    else:
        return f"{row}"


print(f"Processing {datafile} and saving to {savefile1} and {savefile2}")
# Load data from CSV file
# Replace 'data.csv' with the actual path to your CSV file
df = pd.read_csv(datafile)
df2 = pd.read_csv(chou_etal)
df3 = pd.read_csv(scipy_transpose)
matrices = pd.read_csv("data/matrices.csv")

# Strip any whitespace from headers
df.columns = df.columns.str.strip()
df2.columns = df2.columns.str.strip()
df3.columns = df3.columns.str.strip()
matrices.columns = matrices.columns.str.strip()
df["Chou Transpose (ms)"] = df2["Chou Transpose (ms)"]
df["Transpose (ms)"] = df3["Transpose (ms)"]

# remove .mtx from the matrix column
df["Matrix"] = df["Matrix"].str.replace(".mtx", "")

# Calculate the sum of Transpose and Taco for each matrix
df["Transpose + Taco (ms)"] = df["Transpose (ms)"] + df["Taco (ms)"]
df["Chou Transpose + Taco (ms)"] = df["Chou Transpose (ms)"] + df["Taco (ms)"]
df["Taco Transpose + Taco (ms)"] = df["Taco Transpose (ms)"] + df["Taco (ms)"]

df["Density"] = matrices["Nnz"] / (matrices["Rows"] * matrices["Columns"])

# Calculate speedup of Fused over Transpose + Taco
df["Speedup"] = df["Transpose + Taco (ms)"] / df["Fused(Ours) (ms)"]
df["Speedup Chou"] = df["Chou Transpose + Taco (ms)"] / df["Fused(Ours) (ms)"]
df["Speedup Taco Default"] = df["Taco Transpose + Taco (ms)"] / df["Fused(Ours) (ms)"]

pd.set_option("display.max_columns", None)  # Show all columns
print(df)

# Set up the figure and axis
fig, ax1 = plt.subplots(figsize=(14, 8))
# fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(28, 8))

# Set the positions and width for the bars
group_spacing = 0.5
indices = np.arange(len(df)) * (group_spacing + 1)
bar_width = 0.25

# Plot the bar for Fused (Ours)
fused_bars = ax1.bar(indices, df["Fused(Ours) (ms)"], bar_width, label="Ours")

# Plot the stacked bar for Transpose + Taco with Taco at the bottom
taco_bars = ax1.bar(
    indices + bar_width,
    df["Taco (ms)"],
    bar_width,
    label="Taco Execution (ms)",
    color="gray",
)
transpose_bars = ax1.bar(
    indices + bar_width,
    df["Transpose (ms)"],
    bar_width,
    bottom=df["Taco (ms)"],
    label="Scipy Transpose",
)

# Plot the stacked bar for Taco Transpose + Taco with Taco at the bottom
taco_only_bars = ax1.bar(
    indices + 2 * bar_width, df["Taco (ms)"], bar_width, color="gray"
)
taco_transpose_bars = ax1.bar(
    indices + 2 * bar_width,
    df["Chou Transpose (ms)"],
    bar_width,
    bottom=df["Taco (ms)"],
    label="Chou et al. Transpose",
)

ax1.bar(indices + 3 * bar_width, df["Taco (ms)"], bar_width, color="gray")
taco_default_transpose_bars = ax1.bar(
    indices + 3 * bar_width,
    df["Taco Transpose (ms)"],
    bar_width,
    bottom=df["Taco (ms)"],
    label="Taco Default Transpose",
)

# Set log scale for the primary y-axis
ax1.set_yscale("log")
ax1.set_ylabel("Time (ms in Log Scale)")

# Set custom x-axis labels with Matrix and Nnz values
# xtick_labels = [f"{matrix}\nNnz: {convert_rows_to_string(nnz)}, N: {convert_rows_to_string(n)}\nDensity: {'%.2E' % density}" for matrix, nnz, density, n in zip(df['Matrix'], df['Nnz'], df['Density'], df['Rows'])]
xtick_labels = [
    f"{matrix[0:10]}\n{matrix[10:]}" if len(matrix) > 10 else matrix
    for matrix in df["Matrix"]
]
ax1.set_xticks(indices + bar_width)
ax1.set_xticklabels(xtick_labels, rotation=40)

# Set labels and title for primary axis
ax1.set_xlabel("Matrix")
# ax1.set_title('Performance Comparison: Transpose + Taco vs. Fused (Ours) vs. Taco Transpose + Taco with Speedup')
ax1.legend(loc="upper left", ncol=2)
# ax1.set_title('Performance Comparison: Time (ms)')

plt.tight_layout()
plt.savefig(savefile1)

# Add a legend for the primary axis
# ax1.legend(loc="upper center", bbox_to_anchor=(1.05, 1), borderaxespad=0.)


fig, ax2 = plt.subplots(figsize=(14, 8))
# Create a secondary y-axis for the speedup
# ax2 = ax1.twinx()
# ax2.plot(indices + bar_width, df['Speedup Chou'], color='blue', marker='o', label='Speedup (Fused (Ours) vs.\n        [Taco (Chou et al) Transpose + Taco)]')
ax2.bar(
    indices,
    df["Speedup"],
    bar_width,
    label="Speedup vs. [Scipy Transpose + Taco]",
    color="blue",
    alpha=0.7,
)
ax2.bar(
    indices + bar_width,
    df["Speedup Chou"],
    bar_width,
    label="Speedup vs. [Chou et al. Transpose + Taco]",
    color="black",
    alpha=0.7,
)
# ax2.plot(indices + bar_width, df['Speedup'], color='black', marker='o', label='Speedup (Fused (Ours) vs. [Scipy Transpose + Taco)]')
ax2.bar(
    indices + 2 * bar_width,
    df["Speedup Taco Default"],
    bar_width,
    label="Speedup vs. [Taco Default Transpose + Taco]",
    color="green",
    alpha=0.7,
)

ax2.set_yscale("log")
ax2.set_ylabel("Speedup (Log Scale)", color="black")
ax2.tick_params(axis="y", labelcolor="black")
ax2.set_xlabel("Matrix")

ax2.set_xticks(indices + bar_width)
ax2.set_xticklabels(xtick_labels, rotation=40)

# Add a broken horizontal line at speedup = 1
ax2.axhline(y=1, color="red", linestyle="--", linewidth=1, label="Speedup = 1")

# Add legend for the secondary axis
legend = ax2.legend(loc="upper left")
# ax2.set_title('Speedup Comparison')
ax2.grid(True, alpha=0.3, axis="y")

# # Set the color of the first label to white
# text = legend.get_texts()[0]
# text.set_color('white')

# Display the plot
plt.tight_layout()
plt.savefig(savefile2)
