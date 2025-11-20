import argparse
import time
from scipy.io import mmread
from scipy.sparse import csr_matrix
import os

BENCHMARK_FREQUENCEY = 31

# get matrices directory from the environment variable TENSOR_DIR
MATRICES_DIRECTORY = os.environ.get(
    "TENSOR_DIR", "/home/min/a/kadhitha/scratch-space/matrices"
)


def transpose_matrix(filename: str):
    mtx_matrix = mmread(MATRICES_DIRECTORY + "/" + filename)
    csr_matrix = mtx_matrix.tocsr().astype("float32")

    print(f"filename, rows, columns, nnz, time (ms)")
    print(f"{filename}, {csr_matrix.shape[0]}, {csr_matrix.shape[1]}, ", end="")

    execution_times = []
    for _ in range(BENCHMARK_FREQUENCEY):
        start_time = time.time()
        transposed_matrix = csr_matrix.transpose().tocsr()
        end_time = time.time()
        execution_times.append(end_time - start_time)

    # get median of execution times
    elapsed_time = sorted(execution_times)[len(execution_times) // 2]
    print(f"{transposed_matrix.nnz}, {1000*elapsed_time:.6f}")


if __name__ == "__main__":

    # add argument parser take filename -f as input
    argumentParser = argparse.ArgumentParser()
    argumentParser.add_argument(
        "-f", "--filename", help="Enter the filename of the matrix to be transposed"
    )
    args = argumentParser.parse_args()
    filename = args.filename
    transpose_matrix(filename)
