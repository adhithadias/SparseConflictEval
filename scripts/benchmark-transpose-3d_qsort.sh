#!/bin/bash

if [ -z "$TENSOR_DIR" ]; then
    export TENSOR_DIR=/home/min/a/kadhitha/scratch-space/tns
fi

touch ./data/d3_coo_qsort.csv
echo "tensor, sort time all(ms), sort only(ms)" > ./data/d3_coo_qsort.csv
echo "tensor, d0, d1, d2, nnz, sparsity" > ./data/tensor-stats.csv
tensors=(
    vast-2015-mc1-3d 
    darpa1998 
    nell-2 
    freebase_music 
    flickr-3d 
    freebase_sampled 
    nell-1
)

for tensor in "${tensors[@]}"
do
    echo "Running sort3_qsort for $tensor"
    output=$(./build/bin/sort3_qsort -f $TENSOR_DIR/$tensor.tns)

    # read the results file and extract the last line with timings xxx.xxx, xxx.xxx, xxx.xxx
    # and append it to a .csv file with the tensor name as the first column
    line=$(echo "$output" | tail -n 1)
    tensor_dims=$(echo "$output" | tail -n 3 | head -n 1)
    echo "Tensor dimensions: $tensor_dims"
    echo "$tensor, $line" >> ./data/d3_coo_qsort.csv
    echo "$tensor, $tensor_dims" >> ./data/tensor-stats.csv
done
echo "Results saved to data/d3_coo_qsort.csv"
echo "Tensor stats saved to data/tensor-stats.csv"

