#!/bin/bash

if [ -z "$TENSOR_DIR" ]; then
    export TENSOR_DIR=~/scratch-space/suitesparse
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

output_file="data/chou-csr-to-csc.csv"
outputs="Matrix, Chou Transpose (ms)\n"

for matrix in "${matrices[@]}"; do
    echo "Processing matrix: $matrix"
    # ./build/bin/dotprod-denseout -f "$TENSOR_DIR/$matrix/$matrix.mtx"
    output=$(./build/bin/taco-conversion "$TENSOR_DIR/$matrix/$matrix.mtx" 2)
    wait
    echo "$output"
    # get the last line of the output
    last_line=$(echo "$output" | tail -n 2)
    # remove (ms) from the last line
    last_line=$(echo "$last_line" | sed 's/ (ms)//g')
    last_line="$matrix.mtx, $last_line"
    outputs+="$last_line\n"
    echo "$last_line"
done

echo -e "$outputs" > "$output_file"
echo "Results saved to $output_file"