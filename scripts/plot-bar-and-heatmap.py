import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns

# Read the data
df = pd.read_csv('/home/min/a/kadhitha/scratch-space/transpose-fused/data/hadamard-spmm.csv')

# Clean column names (remove extra spaces)
df.columns = df.columns.str.strip()

# Create figure with subplots
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 6))

# 1. Grouped Bar Plot
# Group by density for plotting
densities = df['Density'].unique()
x = np.arange(len(df['Size'].unique()))
width = 0.35

# Get unique sizes for x-axis labels
sizes = df['Size'].unique()

for i, density in enumerate(densities):
    density_data = df[df['Density'] == density]
    
    # Create subplot for each density
    plt.figure(figsize=(12, 8))
    
    x_pos = np.arange(len(density_data))
    width = 0.35
    
    # Bar 1: Ours
    bars1 = plt.bar(x_pos - width/2, density_data['Ours (ms)'], width, 
                   label='Ours', alpha=0.8)
    
    # Bar 2: Hadamard + Matmul (stacked)
    bars2_bottom = plt.bar(x_pos + width/2, density_data['Hadamard (ms)'], width,
                          label='Hadamard', alpha=0.8)
    bars2_top = plt.bar(x_pos + width/2, density_data['Matmul (ms)'], width,
                       bottom=density_data['Hadamard (ms)'], label='Matmul', alpha=0.8)
    
    plt.xlabel('Matrix Size')
    plt.ylabel('Time (ms)')
    plt.title(f'Performance Comparison - Density {density}')
    plt.xticks(x_pos, density_data['Size'])
    plt.legend()
    plt.yscale('log')  # Log scale for better visualization
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.show()

# 2. Speedup Heatmap
# Calculate speedup (Taco / Ours)
df['Speedup'] = df['Taco (ms)'] / df['Ours (ms)']

# Create pivot table for heatmap
heatmap_data = df.pivot(index='Density', columns='Size', values='Speedup')

plt.figure(figsize=(12, 8))
sns.heatmap(heatmap_data, annot=True, fmt='.2f', cmap='RdYlBu_r', 
            cbar_kws={'label': 'Speedup (Taco/Ours)'})
plt.title('Speedup Heatmap: Taco vs Ours')
plt.xlabel('Matrix Size')
plt.ylabel('Density')
plt.tight_layout()
plt.savefig('/home/min/a/kadhitha/scratch-space/transpose-fused/images/hadamard-spmm-heatmap.pdf')

# Alternative: Single figure with all density comparisons
fig, axes = plt.subplots(2, 2, figsize=(16, 12))
axes = axes.flatten()

for i, density in enumerate(densities):
    density_data = df[df['Density'] == density]
    
    x_pos = np.arange(len(density_data))
    width = 0.35
    
    # Bar 1: Ours
    axes[i].bar(x_pos - width/2, density_data['Ours (ms)'], width, 
               label='Ours', alpha=0.8, color='steelblue')
    
    # Bar 2: Hadamard + Matmul (stacked)
    axes[i].bar(x_pos + width/2, density_data['Hadamard (ms)'], width,
               label='Hadamard', alpha=0.8, color='orange')
    axes[i].bar(x_pos + width/2, density_data['Matmul (ms)'], width,
               bottom=density_data['Hadamard (ms)'], label='Matmul', alpha=0.8, color='green')
    
    axes[i].set_xlabel('Matrix Size')
    axes[i].set_ylabel('Time (ms)')
    axes[i].set_title(f'Density {density}')
    axes[i].set_xticks(x_pos)
    axes[i].set_xticklabels(density_data['Size'])
    axes[i].set_yscale('log')
    axes[i].legend()
    axes[i].grid(True, alpha=0.3)

plt.suptitle('Performance Comparison Across Different Densities')
plt.tight_layout()
plt.savefig('/home/min/a/kadhitha/scratch-space/transpose-fused/images/hadamard-spmm-bar-plot.pdf')