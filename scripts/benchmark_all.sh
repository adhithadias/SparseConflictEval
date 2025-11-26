#!/bin/bash

# This script runs all benchmark tests and summarizes the results.

# get TNS and MTX tensor directories from environment variables or use defaults
TNS_DIR=${TNS_DIR:-/artefact/tensors/tns}
MTX_DIR=${MTX_DIR:-/artefact/tensors/mtx}
IMAGES_DIR=${IMAGES_DIR:-/artefact/SparseConflictEval/images}
DATA_DIR=${DATA_DIR:-/artefact/SparseConflictEval/data}

# Run tensor transpose benchmarks
TENSOR_DIR=$MTX_DIR bash scripts/benchmark_scipy_transpose.sh
TENSOR_DIR=$MTX_DIR bash scripts/benchmark_chou_taco_conversion.sh
TENSOR_DIR=$TNS_DIR bash scripts/benchmark-transpose-3d_qsort.sh

# Fig 14, Table 1, and Table 2 stats
TENSOR_DIR=$MTX_DIR IMAGES_DIR=$IMAGES_DIR DATA_DIR=$DATA_DIR bash scripts/benchmark_mtx_row_stats.sh

# Fig 11, and 12, results are saved to /artefact/SparseConflictEval/images/fig11 and /artefact/SparseConflictEval/images/fig12
TENSOR_DIR=$MTX_DIR bash scripts/benchmark_elementwise-mul.sh 0
TENSOR_DIR=$MTX_DIR bash scripts/benchmark_elementwise-mul.sh 1
# Fig 15, results are saved to /artefact/SparseConflictEval/images/fig15
TENSOR_DIR=$MTX_DIR bash scripts/benchmark_dotprod_denseout.sh

# Fig 16, results are saved to /artefact/SparseConflict/images/fig16
TENSOR_DIR=$TNS_DIR bash scripts/benchmark-tensorcontract-3d.sh

# Fig 18
# results are written to data/hadamard-spmm.csv
# images are created in images/hadamard-spmm-bar-plot.pdf and 
# images/hadamard-spmm-heatmap.pdf
bash scripts/benchmark_hadamard_spmm.sh