#!/bin/bash

# get TACO root from the environment variable, if not set, use default path
if [ -z "$TACO_ROOT" ]; then
  export TACO_ROOT=/home/min/a/kadhitha/scratch-space/taco
fi

cwd=$(pwd)
mkdir -p $TACO_ROOT/build
cd $TACO_ROOT/build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j8
cd $cwd