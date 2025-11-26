#!/bin/bash

# export NEW_TACO_COMPILER=/home/min/a/kadhitha/scratch-space/fused-compiler/taco-transpose-fused
# export PATH=$NEW_TACO_COMPILER/buid/bin/:$PATH
# export LD_LIBRARY_PATH=$NEW_TACO_COMPILER/build/lib:$LD_LIBRARY_PATH

echo "Starting batched-dotprod benchmarks..."

# if TENSOR_DIR is not set, set it to default path
if [ -z "$TENSOR_DIR" ]; then
    export TENSOR_DIR=/home/min/a/kadhitha/scratch-space/matrices
fi
# ./build/bin/dotprod-denseout -f ~/scratch-space/suitesparse/Lin/Lin.mtx

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

output_file="data/dotprod-dense.csv"
outputs="Matrix, Taco Transpose (ms), Taco (ms), Fused(Ours) (ms)\n"

for matrix in "${matrices[@]}"; do
    echo "Processing matrix: $matrix"
    # ./build/bin/dotprod-denseout -f "$TENSOR_DIR/$matrix/$matrix.mtx"
    output=$(./build/bin/dotprod-denseout -f "$TENSOR_DIR/$matrix.mtx")
    # echo "$output"
    wait
    # get the last line of the output
    last_line=$(echo "$output" | tail -n 1)
    last_line="$matrix.mtx, $last_line"
    outputs+="$last_line\n"
    # echo "$last_line"
done

printf '%b' "$outputs"
echo -e "$outputs" > "$output_file"
echo "Results saved to $output_file"

# 1 is dotprod-denseout
python3 scripts/matrix-plot.py 1 fig15

echo "\n\n"