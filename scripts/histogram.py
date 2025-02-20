from scipy.io import mmread
import sys
import matplotlib.pyplot as plt
import csv
import numpy as np

ROOT_DIRECTORY = "/home/min/a/kadhitha/scratch-space"
MATRICES_DIRECTORY = f"{ROOT_DIRECTORY}/matrices"
CODE_DIRECTORY = f"{ROOT_DIRECTORY}/transpose-fused"
individual_plots = True
log = False

if not log:
    hist_dir = "histogram"
    stat_file = "matrix_statistics.csv"
else:
    hist_dir = "histogram_log"
    stat_file = "matrix_statistics_log.csv"

def count_nonzeros_per_row(mtx):
    try:
        mtx_file = f"{MATRICES_DIRECTORY}/{mtx}.mtx"
        # Load the sparse matrix from the MTX file
        matrix = mmread(mtx_file).tocsr()  # Convert to Compressed Sparse Row format
        
        # Count non-zeros per row
        nnz_per_row = matrix.getnnz(axis=1)
        if log:
            nnz_per_row = np.ceil(np.log(nnz_per_row + 1)).astype(int)

        # Print results
        # for row_index, nnz in enumerate(nnz_per_row):
        #     print(f"Row {row_index}: {nnz} non-zero values")

        if individual_plots:
            # Create a histogram
            plt.hist(nnz_per_row, bins=range(1, max(nnz_per_row)+2), edgecolor='black')
            plt.xlabel('Number of Non-Zero Values')
            plt.ylabel('Number of Rows')
            plt.title('Histogram of Non-Zero Counts per Row')
            plt.savefig(f'{CODE_DIRECTORY}/images/{hist_dir}/histo_{mtx}.pdf')
            # plt.show()
            
            plt.clf()
        
        # Calculate statistics
        min_nnz = np.min(nnz_per_row)
        max_nnz = np.max(nnz_per_row)
        median_nnz = np.median(nnz_per_row)
        avg_nnz = f"{np.mean(nnz_per_row):.2f}"
        std_nnz = f"{np.std(nnz_per_row):.2f}"
        
        print(f"Statistics for {mtx_file}:")
        print(f"Min: {min_nnz}, Max: {max_nnz}, Median: {median_nnz}, Average: {avg_nnz}, Std Dev: {std_nnz}")

        return nnz_per_row, (f"{mtx}.mtx", min_nnz, max_nnz, median_nnz, avg_nnz, std_nnz)
    except Exception as e:
        print(f"Error reading the file or processing the matrix: {e}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python script_name.py <mtx_file1> <mtx_file2> ...")
    else:
        csv_file = f"{CODE_DIRECTORY}/data/{stat_file}"
        
        if not individual_plots:
            num_files = len(sys.argv) - 1
            cols = 4 if num_files > 4 else 1
            rows = (num_files + cols - 1) // cols

            fig, axes = plt.subplots(rows, cols, figsize=(5 * cols, 4 * rows), squeeze=False)
        
        
        plt.subplots_adjust(hspace=0.4)
        with open(csv_file, mode='w', newline='') as file:
            writer = csv.writer(file)
            writer.writerow(["Matrix File", "Min Non-Zero", "Max Non-Zero", "Median Non-Zero", "Average Non-Zero", "Std Dev Non-Zero"])

            for idx, mtx_file in enumerate(sys.argv[1:]):
                nnz_per_row, stats = count_nonzeros_per_row(mtx_file)
                if stats:
                    writer.writerow(stats)
                    
                if not individual_plots:
                    # Plot histogram in subplot
                    ax = axes[idx // cols, idx % cols]
                    ax.hist(nnz_per_row, bins=range(1, max(nnz_per_row) + 2), edgecolor='black')
                    ax.set_title(f"{mtx_file}")
                    ax.set_xlabel('Non-Zero Count')
                    ax.set_ylabel('Number of Rows')
                
            if not individual_plots:
                # Hide unused subplots
                for idx in range(len(sys.argv) - 1, rows * cols):
                    fig.delaxes(axes[idx // cols, idx % cols])
                    
                plt.savefig(f'{CODE_DIRECTORY}/images/{hist_dir}/histo_all.pdf')

        print(f"Statistics saved to {csv_file}")
