import numpy as np
import sys
import csv
import matplotlib.pyplot as plt
from collections import defaultdict

ROOT_DIRECTORY = "/home/min/a/kadhitha/scratch-space"
MATRICES_DIRECTORY = f"{ROOT_DIRECTORY}/transpose-fused/tensors"
MATRICES_DIRECTORY = f"{ROOT_DIRECTORY}/tns"
CODE_DIRECTORY = f"{ROOT_DIRECTORY}/transpose-fused"
individual_plots = True
log = False

if not log:
    hist_dir = "histograms/tns/not-log"
    stat_file = "tensor_statistics_tns.csv"
else:
    hist_dir = "histograms/tns/log"
    stat_file = "tensor_statistics_tns_log.csv"

def read_tns_file(tns_file):
    """
    Reads a .tns tensor file and converts it to a dictionary-based sparse format.
    Each line in the file is expected to be of the form: i j k value
    """
    tensor = defaultdict(lambda: defaultdict(lambda: defaultdict(float)))
    
    tns_file = f"{MATRICES_DIRECTORY}/{tns_file}.tns"
    
    try:
        with open(tns_file, 'r') as file:
            for line in file:
                indices = line.strip().split()
                if len(indices) != 4:
                    continue  # Skip malformed lines
                i, j, k, value = int(indices[0]), int(indices[1]), int(indices[2]), float(indices[3])
                tensor[i][j][k] = value
        
        return tensor
    except Exception as e:
        print(f"Error reading the .tns file: {e}")
        return None

def count_nonzeros_per_dim2(tensor):
    """
    Counts the number of non-zero values in the second dimension (j-index).
    """
    counts = []
    
    for i in tensor:
        for j in tensor[i]:
            counts.append(len(tensor[i][j]))
    
    return counts

def save_statistics(nnz_per_dim2, tns_file, csv_file):
    """
    Saves min, max, avg, std of non-zeros per second dimension to a CSV file.
    """
    csv_file = f"{CODE_DIRECTORY}/data/{csv_file}"
    values = nnz_per_dim2
    
    if not values:
        print(f"No non-zero values found in {tns_file}")
        return
    
    min_nnz = np.min(values)
    max_nnz = np.max(values)
    median_nnz = np.median(values)
    avg_nnz = f"{np.mean(values):.2f}"
    std_nnz = f"{np.std(values):.2f}"

    print(f"Statistics for {tns_file}: Min={min_nnz}, Max={max_nnz}, Median={median_nnz}, Avg={avg_nnz}, Std={std_nnz}")

    with open(csv_file, mode='a', newline='') as file:
        writer = csv.writer(file)
        writer.writerow([tns_file, min_nnz, max_nnz, avg_nnz, std_nnz])

def plot_histogram(nnz_per_dim2, tns_file):
    """
    Plots a histogram of non-zero counts in the second dimension.
    """
    values = nnz_per_dim2
    values = [value if value <= 1000 else 1001 for value in values]
    
    plt.figure(figsize=(8, 5))
    plt.hist(values, bins=range(1, max(values) + 2), edgecolor='black')
    plt.xlabel('Number of Non-Zero Values (Dimension 2)')
    plt.ylabel('Frequency')
    plt.title(f'Histogram of Non-Zero Counts in Second Dimension ({tns_file})')
    # plt.show()
    plt.savefig(f'{CODE_DIRECTORY}/images/{hist_dir}/histo_{tns_file}.pdf')

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python script_name.py <tns_file1> <tns_file2> ...")
    else:
        csv_file = "tensor_statistics_tns.csv"
        
        # Initialize CSV file
        with open(csv_file, mode='w', newline='') as file:
            writer = csv.writer(file)
            writer.writerow(["Tensor File", "Min", "Max", "Median", "Average", "Std Dev"])

        for tns_file in sys.argv[1:]:
            tensor = read_tns_file(tns_file)
            if tensor:
                nnz_per_dim2 = count_nonzeros_per_dim2(tensor)
                save_statistics(nnz_per_dim2, tns_file, csv_file)
                plot_histogram(nnz_per_dim2, tns_file)
        
        print(f"Statistics saved to {csv_file}")