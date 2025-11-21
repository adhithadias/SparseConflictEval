#!/bin/bash

# use ./bin/benchmark_spm_spm_spm -n 10000 -d 0.01 -r 10 
# execute for different pairs of (n, d), pairs are (1000, 0.1), (10000, 0.01), (100000, 0.001), (1000000, 0.0001)
# for each pair, execute for r rounds and read the output from the terminal and save it to a file
# there are two outputs Fused median: X ms and Taco median: Y ms in two lines

if [ -z "$TACO_ROOT" ]; then
    export TACO_ROOT=/home/min/a/kadhitha/scratch-space/fused-compiler/taco-transpose-fused
fi

export PATH=$TACO_ROOT/build/bin/:$PATH
export LD_LIBRARY_PATH=$TACO_ROOT/build/lib:$LD_LIBRARY_PATH

val1=(1024 2048 4096 8192 16384 32768)
val2=(0.05 0.01 0.001 0.0001)
r=10    

mkdir -p tmp

# write Size, Density, Ours (ms), Hadamard (ms), Matmul (ms), Taco (ms) to data/hadamard-spmm.csv
echo "Size,Density,Ours (ms),Hadamard (ms),Matmul (ms),Taco (ms)" > data/hadamard-spmm.csv

# iterate over all combinations of val1 and val2
for i in {0..3}
do
    for j in {0..5}
    do
        n=${val1[$j]}
        d=${val2[$i]}
        echo "Running for n=$n, d=$d"
        output=$(./build/bin/benchmark_spm_spm_spm -n $n -d $d -r $r)
        last_line=$(echo "$output" | tail -n 1)
        echo "$last_line"
        echo "$last_line" >> data/hadamard-spmm.csv
        # ./build/bin/benchmark_spm_spm_spm -n $n -d $d -r $r | tee -a tmp/benchmark_spm_spm_spm_results.txt
    done
done
echo "Results saved to data/hadamard-spmm.csv"

python3 scripts/plot-bar-and-heatmap.py