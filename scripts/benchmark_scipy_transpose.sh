#!/bin/bash

echo "Starting SciPy transpose benchmarks..."

# keep matrices bcsstk17 scircuit mac_econ_fwd500 majorbasis Lin rma10 cop20k_A webbase\-1M cant pdb1HYS ecology1 largebasis consph shipsec1 atmosmodd pwtk in an array
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

outputs="Matrix, M, N, nnz, Transpose (ms)\n"
# execute python scripts/transpose.py for each matrix and get the output
for matrix in "${matrices[@]}"; do
    echo "Processing matrix: $matrix"
    output=$(python3 scripts/scipy_transpose.py -f "$matrix".mtx 2>&1)
    wait
    # get the last line of the output
    last_line=$(echo "$output" | tail -n 1)
    # echo "$last_line"
    outputs+="$last_line\n"
done

mkdir -p data
printf '%b' "$outputs"
echo -e "$outputs" > data/scipy-csr-to-csc.csv
echo "\n\n"