!#/bin/bash

cwd=$(pwd)
mkdir -p /home/min/a/kadhitha/scratch-space/taco/build
cd /home/min/a/kadhitha/scratch-space/taco/build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j8
cd $cwd