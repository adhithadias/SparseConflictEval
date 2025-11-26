#!/bin/bash

# SUITESPARSE_DIR=~/scratch-space/suitesparse
# ./build/bin/dotprod-denseout -f ~/scratch-space/suitesparse/Lin/Lin.mtx

# get TENSOR_DIR from environment variable, if not set, use default path
if [ -z "$TENSOR_DIR" ]; then
    TENSOR_DIR=/home/min/a/kadhitha/scratch-space/matrices
fi

if [ -z "$IMAGES_DIR" ]; then
    export IMAGES_DIR=/home/min/a/kadhitha/scratch-space/transpose-fused/images
fi

if [ -z "$DATA_DIR" ]; then
    export DATA_DIR=/home/min/a/kadhitha/scratch-space/transpose-fused/data
fi

matrices=(
    "bcsstk17"
    "scircuit"
    "mac_econ_fwd500"
    "majorbasis"
    "Lin"
    "rma10"
    "cop20k_A"
    "webbase-1M"
    "cant"
    "pdb1HYS"
    "ecology1"
    "largebasis"
    "consph"
    "shipsec1"
    "atmosmodd"
    "pwtk"
)

output_file="data/matrix-stats.csv"
outputs="Matrix, Rows, Cols, Nnz, min_row_nnz, max_row_nnz, median_row_nnz, avg_row_nnz, std_row_nnz\n"

# echo $outputs > $output_file

for matrix in "${matrices[@]}"; do
    echo "Processing matrix: $matrix"
    # ./build/bin/dotprod-denseout -f "$TENSOR_DIR/$matrix.mtx"
    output=$(python3 scripts/mtx_row_stats.py "$TENSOR_DIR/$matrix.mtx")
    wait
    # echo "$output"
    last_line=$(echo "$output" | tail -n 1)
    last_line="$matrix, $last_line"
    # echo $last_line >> $output_file
    # echo "$last_line"
    outputs+="$last_line\n"
done

mkdir -p data
printf '%b' "$outputs"
echo -e "$outputs" > "$output_file"
echo "Results saved to $output_file"