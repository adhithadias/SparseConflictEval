import matplotlib.pyplot as plt
import pandas as pd

# Define the dataset
data = {
    "benchmark": ["<SDDMM,\n SpMM>", "<SpMMH,\n GEMM>", "<SpMM,\n GEMM>", "<SDDMM,\n SpMM,\n GEMM>", "<MTTKRP,\n GEMM>", "<SpTTM,\n SpTTM>"],
    "range1": ["0.91-1.5", "1.60-1.99", "1.23-3.27", "1.20-2.26", "0.85-1.86", "0.27-2.08"],
    "range2": ["10.75-33.39", "1.29-50.55", "6.91-79.86", "93-1997", "27.05-89.12", "1.88-66.20"]
}

# Convert the dataset to a pandas DataFrame
df = pd.DataFrame(data)

# Extract the range values and convert them to floats
range1_values = [list(map(float, r.split('-'))) for r in df['range1']]
range2_values = [list(map(float, r.split('-'))) for r in df['range2']]

# Create a figure and axis
fig, ax = plt.subplots()

# Plot the range bars
for i, (r1, r2) in enumerate(zip(range1_values, range2_values)):
    ax.plot([i+0.1, i+0.1], r2, color='none', linewidth=5, label=r'$\frac{TACO-Original}{SparseLNR}$' if i == 0 else None)
    ax.plot([i, i], r1, color='blue', linewidth=5, label=r'$\frac{TACO-Seperate}{SparseLNR}$' if i == 0 else None)

# Set the x-axis ticks and labels
ax.set_xticks(range(len(df)))
ax.set_xticklabels(df['benchmark'], rotation=0)

# Add horizontal lines at 1 and 0.5
ax.axhline(1, color='green', linestyle='--', linewidth=2, label='1')
# ax.axhline(0.5, color='orange', linestyle='--', linewidth=2, label='0.5')

# fig.legend(loc='upper right', bbox_to_anchor=(1.05, 1))
fig.legend(bbox_to_anchor=(0.35, 0.88), loc='upper center', ncol=2, fancybox=True, shadow=True, fontsize=14)

# Set the title and labels
ax.set_title('Speedup of SparseLNR against TACO\n(Single-thread Execution)')
ax.set_xlabel('Benchmarks')
ax.set_ylabel('Speedup (log scale)')

ax.set_yscale('log')

# Show the plot
plt.tight_layout()
plt.savefig('sparselnr-vs-taco-single.png')