
#!/bin/bash

export LD_LIBRARY_PATH=$TACO_ROOT/build/lib:$LD_LIBRARY_PATH

mkdir -p build
cd build
cmake ..
make -j8
cd ..
