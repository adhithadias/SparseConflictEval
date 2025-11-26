#!/bin/bash

# This script runs some benchmark tests for kick-the-tires phase.

# get TNS and MTX tensor directories from environment variables or use defaults
TNS_DIR=${TNS_DIR:-/artefact/tensors/tns}
MTX_DIR=${MTX_DIR:-/artefact/tensors/mtx}
IMAGES_DIR=${IMAGES_DIR:-/artefact/SparseConflictEval/images}
DATA_DIR=${DATA_DIR:-/artefact/SparseConflictEval/data}

# Run tensor transpose benchmarks
TENSOR_DIR=$MTX_DIR bash scripts/benchmark_scipy_transpose.sh
TENSOR_DIR=$MTX_DIR bash scripts/benchmark_chou_taco_conversion.sh

# Fig 14, Table 1, and Table 2 stats
TENSOR_DIR=$MTX_DIR IMAGES_DIR=$IMAGES_DIR DATA_DIR=$DATA_DIR bash scripts/benchmark_mtx_row_stats.sh

# Fig 15, results are saved to /artefact/SparseConflictEval/images/fig15
TENSOR_DIR=$MTX_DIR bash scripts/benchmark_dotprod_denseout.sh
