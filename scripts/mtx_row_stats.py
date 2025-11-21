import sys
import os
import numpy as np
from scipy.io import mmread
from scipy.sparse import issparse
import matplotlib.pyplot as plt
import matplotlib
from enum import Enum

class FontSize(Enum):
    SMALL_SIZE = 8
    MEDIUM_SIZE = 12
    BIGGER_SIZE = 16
    SUPER_BIG_SIZE = 20
    
plt.rcParams.update({'font.size': FontSize.SUPER_BIG_SIZE.value})

matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42

def plot_row_nonzero_distribution(file_path, nonzero_per_row, avg_row_nnz, median_row_nnz):
    # --- Plotting Row Non-Zero Distribution ---
    # file/path/matrix_name.mtx -> matrix_name
    matrix_name = os.path.splitext(os.path.basename(file_path))[-2]
    
    if (matrix_name not in ["scircuit", "largebasis", "shipsec1", "ecology1", "pdb1HYS"]):
        print("\nSkipping plot for this matrix.")
        return
    
    # get images directory from environment variable or use default
    images_dir = os.getenv('IMAGES_DIR', 'images')
    histo_dir = os.path.join(images_dir, 'fig14')
    if not os.path.exists(histo_dir):
        os.makedirs(histo_dir, exist_ok=True)
        print(f"Created histogram images directory: {histo_dir}")
    
    fig_filename = f"{histo_dir}/{matrix_name}_row_nonzero_distribution.pdf"
    plt.figure(figsize=(10, 6))
    
    # Determine number of bins. Using the square root of the number of rows is a common rule of thumb.
    num_bins = int(np.sqrt(len(nonzero_per_row)))
    
    # Determine number of bins using the Freedman-Diaconis rule for better adaptability.
    q75, q25 = np.percentile(nonzero_per_row, [75, 25])
    iqr = q75 - q25
    if iqr > 0:
        bin_width = 2 * iqr / (len(nonzero_per_row)**(1/3))
        data_range = nonzero_per_row.max() - nonzero_per_row.min()
        if bin_width > 0:
            num_bins = int(np.ceil(data_range / bin_width))
        else: # data_range is 0
            num_bins = 1
    else:
        # Fallback to square-root rule if IQR is 0
        num_bins = int(np.sqrt(len(nonzero_per_row)))

    # Set a reasonable upper and lower limit for the number of bins
    num_bins = max(2, min(num_bins, 350))
    
    if matrix_name == "scircuit":
        num_bins = 350
    elif matrix_name == "largebasis":
        num_bins = 10
    elif matrix_name == "shipsec1":
        num_bins = 70
    elif matrix_name == "ecology1":
        num_bins = 2
    elif matrix_name == "pdb1HYS":
        num_bins = 220
    
    print(f"\nPlotting histogram with {num_bins} bins...")
    plt.hist(nonzero_per_row, bins=num_bins, edgecolor='black', color='skyblue')
    
    plt.axvline(avg_row_nnz, color='red', linestyle='dashed', linewidth=3, label=f'Mean: {avg_row_nnz:.2f}')
    plt.axvline(median_row_nnz, color='green', linestyle='dashed', linewidth=3, label=f'Median: {median_row_nnz:.1f}')
    
    plt.title('Distribution')
    plt.xlabel('Number of Non-Zero Values in Row')
    plt.ylabel('Number of Rows (Frequency)')
    plt.legend()
    plt.grid(axis='y', alpha=0.5)
    
    # Format y-axis to use scientific notation for large numbers
    plt.ticklabel_format(style='sci', axis='y', scilimits=(3,3))
    
    try:
        plt.savefig(fig_filename)
        print(f"\nSuccessfully saved distribution plot to: {fig_filename}")
    except Exception as e:
        print(f"\nError saving plot: {e}", file=sys.stderr)
    finally:
        plt.close()

def count_non_zero_per_row_and_col(file_path):
    """
    Loads a sparse matrix from an .mtx file and calculates the number of
    non-zero elements in each row and column, including statistical summaries.

    Args:
        file_path (str): The path to the Matrix Market (.mtx) file.
    """
    if not os.path.exists(file_path):
        print(f"Error: File not found at '{file_path}'", file=sys.stderr)
        sys.exit(1)

    print(f"Loading sparse matrix from: {file_path}")
    
    try:
        # mmread loads the matrix, typically into a coo_matrix or csc_matrix.
        matrix = mmread(file_path)
    except Exception as e:
        print(f"Error reading .mtx file: {e}", file=sys.stderr)
        sys.exit(1)

    if not issparse(matrix):
        print("Warning: Matrix loaded is not sparse. Converting to sparse CSR format.", file=sys.stderr)
        matrix = matrix.tocsr() 
        
    num_rows, num_cols = matrix.shape

    # --- Count non-zero elements per row (using CSR format) ---
    csr_matrix = matrix.tocsr()
    # np.diff(indptr) gives the number of stored non-zero elements in each row
    nonzero_per_row = np.diff(csr_matrix.indptr)
    
    # --- Count non-zero elements per column (using CSC format) ---
    csc_matrix = matrix.tocsc()
    # np.diff(indptr) gives the number of stored non-zero elements in each column
    nonzero_per_col = np.diff(csc_matrix.indptr)

    # --- Calculate Statistics ---
    
    # Matrix dimensions and nnz
    nnz = matrix.nnz
    
    # Row Statistics
    min_row_nnz = nonzero_per_row.min()
    max_row_nnz = nonzero_per_row.max()
    avg_row_nnz = nonzero_per_row.mean()
    median_row_nnz = np.median(nonzero_per_row)
    std_row_nnz = np.std(nonzero_per_row)  # Added Standard Deviation
    
    # Column Statistics
    min_col_nnz = nonzero_per_col.min()
    max_col_nnz = nonzero_per_col.max()
    avg_col_nnz = nonzero_per_col.mean()
    median_col_nnz = np.median(nonzero_per_col)
    std_col_nnz = np.std(nonzero_per_col)  # Added Standard Deviation

    # --- Print Results ---
    
    print("\n--- Non-Zero Counts Per Row ---")
    
    # Print the first 10 row counts
    for i, count in enumerate(nonzero_per_row[:10]):
        print(f"Row {i}: {count}")
    if len(nonzero_per_row) > 10:
        print(f"... (showing first 10 out of {len(nonzero_per_row)} rows)")

    print(f"\nTotal Rows: {csr_matrix.shape[0]}")
    print(f"Minimum Non-Zero Count per Row: {min_row_nnz}")
    print(f"Maximum Non-Zero Count per Row: {max_row_nnz}")
    print(f"Average Non-Zero Count per Row: {avg_row_nnz:.2f}")
    print(f"Median Non-Zero Count per Row: {median_row_nnz:.1f}")
    print(f"Standard Deviation per Row: {std_row_nnz:.2f}")

    print("\n--- Non-Zero Counts Per Column ---")
    
    # Print the first 10 column counts
    for j, count in enumerate(nonzero_per_col[:10]):
        print(f"Column {j}: {count}")
    if len(nonzero_per_col) > 10:
        print(f"... (showing first 10 out of {len(nonzero_per_col)} columns)")

    print(f"\nTotal Columns: {csc_matrix.shape[1]}")
    print(f"Minimum Non-Zero Count per Column: {min_col_nnz}")
    print(f"Maximum Non-Zero Count per Column: {max_col_nnz}")
    print(f"Average Non-Zero Count per Column: {avg_col_nnz:.2f}")
    print(f"Median Non-Zero Count per Column: {median_col_nnz:.1f}")
    print(f"Standard Deviation per Column: {std_col_nnz:.2f}")
    
    # Sanity check
    print(f"\nMatrix total non-zeros (nnz): {matrix.nnz}")

    # --- Plot Row Non-Zero Distribution ---
    plot_row_nonzero_distribution(file_path, nonzero_per_row, avg_row_nnz, median_row_nnz)
    
    return (int(num_rows), int(num_cols), int(nnz), 
            min_row_nnz, max_row_nnz, median_row_nnz, avg_row_nnz, std_row_nnz,
            min_col_nnz, max_col_nnz, median_col_nnz, avg_col_nnz, std_col_nnz)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python mtx_nonzero_counter.py <path_to_mtx_file>", file=sys.stderr)
        sys.exit(1)
    
    mtx_file_path = sys.argv[1]
    stats = count_non_zero_per_row_and_col(mtx_file_path)
    
    print("\n--- Summary Statistics ---")
    print("num_rows, num_cols, nnz, min_row_nnz, max_row_nnz, median_row_nnz, avg_row_nnz, std_row_nnz")
    print(", ".join(f"{stat:.2f}" if isinstance(stat, float) else f"{stat}" for stat in stats[:8]))