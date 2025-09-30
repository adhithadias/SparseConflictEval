#!/bin/bash

# use ./bin/benchmark_spm_spm_spm -n 10000 -d 0.01 -r 10 
# execute for different pairs of (n, d), pairs are (1000, 0.1), (10000, 0.01), (100000, 0.001), (1000000, 0.0001)
# for each pair, execute for r rounds and read the output from the terminal and save it to a file
# there are two outputs Fused median: X ms and Taco median: Y ms in two lines

export NEW_TACO_COMPILER=/home/min/a/kadhitha/scratch-space/fused-compiler/taco-transpose-fused
export PATH=$NEW_TACO_COMPILER/build/bin/:$PATH
export LD_LIBRARY_PATH=$NEW_TACO_COMPILER/build/lib:$LD_LIBRARY_PATH

val1=(1024 2048 4096 8192 16384 32768 65536)
val2=(0.1 0.1 0.1 0.01 0.01 0.001 0.001)
r=10    

mkdir -p tmp

for i in {0..6}
do
    n=${val1[$i]}
    d=${val2[$i]}
    echo "Running for n=$n, d=$d"
    ./build/bin/benchmark_spm_spm_spm -n $n -d $d -r $r | tee -a tmp/benchmark_spm_spm_spm_results.txt
done
echo "Results saved to tmp/benchmark_spm_spm_spm_results.txt"
