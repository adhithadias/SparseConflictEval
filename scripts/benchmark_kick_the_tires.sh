#!/bin/bash

# This script runs some benchmark tests for kick-the-tires phase.

# get TNS and MTX tensor directories from environment variables or use defaults
TNS_DIR=${TNS_DIR:-/artefact/tensors/tns}
MTX_DIR=${MTX_DIR:-/artefact/tensors/mtx}

# Run tensor transpose benchmarks
TENSOR_DIR=$MTX_DIR bash scripts/benchmark_scipy_transpose.sh

# Fig 11, and 12, results are saved to /artefact/SparseConflictEval/images/fig11
# and /artefact/SparseConflictEval/images/fig12
TENSOR_DIR=$MTX_DIR bash scripts/benchmark_elementwise-mul.sh 0
