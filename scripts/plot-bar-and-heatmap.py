from enum import Enum
import pandas as pd
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
import os

class FontSize(Enum):
    SMALL_SIZE = 8
    MEDIUM_SIZE = 12
    BIGGER_SIZE = 16
    
plt.rcParams.update({'font.size': FontSize.BIGGER_SIZE.value})

matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42

# get DATA_DIR from environment variable, if not set, use default path
data_path = os.getenv('DATA_DIR') # /local/scratch/a/kadhitha/transpose-fused/images
if not data_path:
    # Try to find data directory relative to script location
    script_dir = os.path.dirname(os.path.abspath(__file__))
    data_path = os.path.join(os.path.dirname(script_dir), 'data')
    print(f"DATA_DIR not set, using: {data_path}")

images_path = os.getenv('IMAGES_DIR')
if not images_path:
    script_dir = os.path.dirname(os.path.abspath(__file__))
    images_path = os.path.join(os.path.dirname(script_dir), 'images')
    print(f"IMAGES_DIR not set, using: {images_path}")

# Verify paths exist
if not os.path.isdir(data_path):
    raise FileNotFoundError(f"Data directory not found: {data_path}")
if not os.path.isdir(images_path):
    os.makedirs(images_path, exist_ok=True)
    print(f"Created images directory: {images_path}")


# Read the data
df = pd.read_csv(f'{data_path}/hadamard-spmm.csv')

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
                          label='Hadamard TACO', alpha=0.8)
    bars2_top = plt.bar(x_pos + width/2, density_data['Matmul (ms)'], width,
                       bottom=density_data['Hadamard (ms)'], label='Matmul TACO', alpha=0.8)
    
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
plt.savefig(f'{images_path}/hadamard-spmm-heatmap.pdf')

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
               label='Hadamard TACO', alpha=0.8, color='orange')
    axes[i].bar(x_pos + width/2, density_data['Matmul (ms)'], width,
               bottom=density_data['Hadamard (ms)'], label='Matmul TACO', alpha=0.8, color='green')
    
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
plt.savefig(f'{images_path}/hadamard-spmm-bar-plot.pdf')