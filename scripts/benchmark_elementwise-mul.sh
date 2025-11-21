#!/bin/bash

# export NEW_TACO_COMPILER=/home/min/a/kadhitha/scratch-space/fused-compiler/taco-transpose-fused
# export PATH=$NEW_TACO_COMPILER/buid/bin/:$PATH
# export LD_LIBRARY_PATH=$NEW_TACO_COMPILER/build/lib:$LD_LIBRARY_PATH

# SUITESPARSE_DIR=~/scratch-space/suitesparse
# ./build/bin/dotprod-denseout -f ~/scratch-space/suitesparse/Lin/Lin.mtx

if [ -z "$TENSOR_DIR" ]; then
    export TENSOR_DIR=/home/min/a/kadhitha/scratch-space/matrices
fi

# get -a flag from command line, default is 1 (true)
if [ -z "$1" ]; then
    assembleWhileCompute=1
    echo "No assembleWhileCompute flag specified. Using default true."
else
    assembleWhileCompute=$1
    echo "assembleWhileCompute flag set to: $assembleWhileCompute"
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

if [ "$assembleWhileCompute" -eq 0 ]; then
    output_file="data/elementwise-mul-without.csv"
else
    output_file="data/elementwise-mul-with.csv"
fi
outputs="Matrix, Taco Transpose (ms), Taco (ms), Fused(Ours) (ms)\n"

for matrix in "${matrices[@]}"; do
    echo "Processing matrix: $matrix"
    # ./build/bin/dotprod-denseout -f "$TENSOR_DIR/$matrix/$matrix.mtx"
    output=$(./build/bin/elementwise-mul -f "$TENSOR_DIR/$matrix.mtx" -a "$assembleWhileCompute")
    echo "$output"
    # get the last line of the output
    last_line=$(echo "$output" | tail -n 1)
    last_line="$matrix.mtx, $last_line"
    outputs+="$last_line\n"
    echo "$last_line"
done

echo -e "$outputs" > "$output_file"
echo "Results saved to $output_file"

# 0 is elementwise-mul
# if assembleWhileCompute is 0, then pass fig 11, else fig12
if [ "$assembleWhileCompute" -eq 0 ]; then
    python3 scripts/matrix-plot.py 2 fig11
else
    python3 scripts/matrix-plot.py 0 fig12
fi