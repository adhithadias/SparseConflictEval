#!/bin/bash

# export NEW_TACO_COMPILER=/home/min/a/kadhitha/scratch-space/fused-compiler/taco-transpose-fused
# export PATH=$NEW_TACO_COMPILER/build/bin/:$PATH
# export LD_LIBRARY_PATH=$NEW_TACO_COMPILER/build/lib:$LD_LIBRARY_PATH

if [ -z "$TENSOR_DIR" ]; then
    export TENSOR_DIR=/home/min/a/kadhitha/scratch-space/tns
fi

touch ./data/tensorcontract-1dout.csv
echo "tensor, transpose(ms), taco(ms), ours(ms)" > ./data/tensorcontract-1dout.csv
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
    echo "Running tensorcontract-1dout for $tensor"
    output=$(./build/bin/tensorcontract-1dout -f $TENSOR_DIR/$tensor.tns)

    # read the results file and extract the last line with timings xxx.xxx, xxx.xxx, xxx.xxx
    # and append it to a .csv file with the tensor name as the first column
    line=$(echo "$output" | tail -n 1)
    echo $line
    echo "$tensor, $line" >> ./data/tensorcontract-1dout.csv
done
echo "Results saved to tensorcontract-1dout.csv"

python3 scripts/tensor-plot.py