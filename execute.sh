# execute these in the terminal
export PATH=/home/min/a/kadhitha/scratch-space/taco/build/bin/:$PATH
export LD_LIBRARY_PATH=/home/min/a/kadhitha/scratch-space/taco/build/lib:$LD_LIBRARY_PATH

# new taco compiler path
export NEW_TACO_COMPILER=/home/min/a/kadhitha/scratch-space/fused-compiler/taco-transpose-fused
export PATH=$NEW_TACO_COMPILER/buid/bin/:$PATH
export LD_LIBRARY_PATH=$NEW_TACO_COMPILER/build/lib:$LD_LIBRARY_PATH

PATH=/home/min/a/kadhitha/scratch-space/taco/build/bin/:$PATH LD_LIBRARY_PATH=/home/min/a/kadhitha/scratch-space/taco/build/lib:$LD_LIBRARY_PATH \
g++ -std=c++11 -O3 -DNDEBUG -DTACO -I /home/min/a/kadhitha/scratch-space/taco/include/ -L/home/min/a/kadhitha/scratch-space/taco/build/lib test0.cpp -o test0 -ltaco

PATH=/home/min/a/kadhitha/scratch-space/taco/build/bin/:$PATH LD_LIBRARY_PATH=/home/min/a/kadhitha/scratch-space/taco/build/lib:$LD_LIBRARY_PATH \
g++ -std=c++11 -O3 -DNDEBUG -DTACO -I /home/min/a/kadhitha/scratch-space/taco/include/ -L/home/min/a/kadhitha/scratch-space/taco/build/lib spmv.cpp -o spmv -ltaco

./spmv -i ../matrices/bcsstk17.mtx
./spmv -i ../matrices/kl02.mtx

/local/scratch/a/Suitesparse/kl02/kl02.mtx

# matrices=(bcsstk17 scircuit mac_econ_fwd500 com-Amazon rma10 \
            # cop20k_A webbase-1M cant pdb1HYS consph shipsec1 \
            # pwtk circuit5M)

./build/bin/dotprod-denseout -f ../matrices/bcsstk17.mtx
./test0 -i scircuit.mtx
./test0 -i mac_econ_fwd500.mtx
./test0 -i majorbasis.mtx
./test0 -i Lin.mtx
./test0 -i rma10.mtx
./test0 -i cop20k_A.mtx
./test0 -i webbase-1M.mtx
./test0 -i cant.mtx
./test0 -i pdb1HYS.mtx
./test0 -i ecology1.mtx
./test0 -i consph.mtx
./test0 -i shipsec1.mtx
./test0 -i atmosmodd.mtx
./test0 -i pwtk.mtx
./test0 -i circuit5M.mtx
./test0 -i largebasis.mtx


PATH=/home/min/a/kadhitha/scratch-space/taco/build/bin/:$PATH LD_LIBRARY_PATH=/home/min/a/kadhitha/scratch-space/taco/build/lib:$LD_LIBRARY_PATH \
g++ -std=c++11 -O3 -DNDEBUG -DTACO -I /home/min/a/kadhitha/scratch-space/taco/include/ -L/home/min/a/kadhitha/scratch-space/taco/build/lib test1.cpp -o test1 -ltaco

./build/bin/dotprod-spout -f bcsstk17.mtx
./test1 -i scircuit.mtx
./test1 -i mac_econ_fwd500.mtx
./test1 -i majorbasis.mtx
./test1 -i Lin.mtx
./test1 -i rma10.mtx
./test1 -i cop20k_A.mtx
./test1 -i webbase-1M.mtx
./test1 -i cant.mtx
./test1 -i pdb1HYS.mtx
./test1 -i ecology1.mtx
./test1 -i consph.mtx
./test1 -i shipsec1.mtx
./test1 -i atmosmodd.mtx
./test1 -i pwtk.mtx
./test1 -i circuit5M.mtx
./test1 -i largebasis.mtx


# ./test0 -i com-Amazon.mtx

g++ -std=c++11 -O3 -DNDEBUG -DTACO -I /home/min/a/kadhitha/scratch-space/taco/include/ -L/home/min/a/kadhitha/scratch-space/taco/build/lib ./src/sort.cpp -o sort -ltaco


PATH=/home/min/a/kadhitha/scratch-space/taco/build/bin/:$PATH LD_LIBRARY_PATH=/home/min/a/kadhitha/scratch-space/taco/build/lib:$LD_LIBRARY_PATH \
g++ -std=c++11 -O3 -DNDEBUG -DTACO -I /home/min/a/kadhitha/scratch-space/taco/include/ -L/home/min/a/kadhitha/scratch-space/taco/build/lib test2.cpp -o test2 -ltaco

./build/bin/singlemat-contract -f bcsstk17.mtx
./test2 -i scircuit.mtx
./test2 -i mac_econ_fwd500.mtx
./test2 -i majorbasis.mtx
./test2 -i Lin.mtx
./test2 -i rma10.mtx
./test2 -i cop20k_A.mtx
./test2 -i webbase-1M.mtx
./test2 -i cant.mtx
./test2 -i pdb1HYS.mtx
./test2 -i ecology1.mtx
./test2 -i consph.mtx
./test2 -i shipsec1.mtx
./test2 -i atmosmodd.mtx
./test2 -i pwtk.mtx
./test2 -i circuit5M.mtx
./test2 -i largebasis.mtx


Y(i,j) = A(i,j) * B(j,i)
./test3 -i 494_bus.mtx

./bin/spmspm -f ../tensors/bus.mtx

python scripts/mtx_nonzero_counter.py ~/scratch-space/suitesparse/mac_econ_fwd500/mac_econ_fwd500.mtx

cd /home/min/a/kadhitha/scratch-space/transpose-fused
./build/bin/elementwise-mul -f ~/scratch-space/suitesparse/bcsstk17/bcsstk17.mtx
./build/bin/elementwise-mul -f ~/scratch-space/suitesparse/scircuit/scircuit.mtx
./build/bin/elementwise-mul -f ~/scratch-space/suitesparse/mac_econ_fwd500/mac_econ_fwd500.mtx
./build/bin/elementwise-mul -f ~/scratch-space/suitesparse/majorbasis/majorbasis.mtx
./build/bin/elementwise-mul -f ~/scratch-space/suitesparse/Lin/Lin.mtx
./build/bin/elementwise-mul -f ~/scratch-space/suitesparse/rma10/rma10.mtx
./build/bin/elementwise-mul -f ~/scratch-space/suitesparse/cop20k_A/cop20k_A.mtx
./build/bin/elementwise-mul -f ~/scratch-space/suitesparse/webbase-1M/webbase-1M.mtx
./build/bin/elementwise-mul -f ~/scratch-space/suitesparse/cant/cant.mtx
./build/bin/elementwise-mul -f ~/scratch-space/suitesparse/pdb1HYS/pdb1HYS.mtx
./build/bin/elementwise-mul -f ~/scratch-space/suitesparse/ecology1/ecology1.mtx
./build/bin/elementwise-mul -f ~/scratch-space/suitesparse/consph/consph.mtx
./build/bin/elementwise-mul -f ~/scratch-space/suitesparse/largebasis/largebasis.mtx
./build/bin/elementwise-mul -f ~/scratch-space/suitesparse/shipsec1/shipsec1.mtx
./build/bin/elementwise-mul -f ~/scratch-space/suitesparse/atmosmodd/atmosmodd.mtx
./build/bin/elementwise-mul -f ~/scratch-space/suitesparse/pwtk/pwtk.mtx
./build/bin/elementwise-mul -f ~/scratch-space/suitesparse/circuit5M/circuit5M.mtx

cd /home/min/a/kadhitha/scratch-space/transpose-fused
./build/bin/dotprod-denseout -f ~/scratch-space/suitesparse/bcsstk17/bcsstk17.mtx
./build/bin/dotprod-denseout -f ~/scratch-space/suitesparse/scircuit/scircuit.mtx
./build/bin/dotprod-denseout -f ~/scratch-space/suitesparse/mac_econ_fwd500/mac_econ_fwd500.mtx
./build/bin/dotprod-denseout -f ~/scratch-space/suitesparse/majorbasis/majorbasis.mtx
./build/bin/dotprod-denseout -f ~/scratch-space/suitesparse/Lin/Lin.mtx
./build/bin/dotprod-denseout -f ~/scratch-space/suitesparse/rma10/rma10.mtx
./build/bin/dotprod-denseout -f ~/scratch-space/suitesparse/cop20k_A/cop20k_A.mtx
./build/bin/dotprod-denseout -f ~/scratch-space/suitesparse/webbase-1M/webbase-1M.mtx
./build/bin/dotprod-denseout -f ~/scratch-space/suitesparse/cant/cant.mtx
./build/bin/dotprod-denseout -f ~/scratch-space/suitesparse/pdb1HYS/pdb1HYS.mtx
./build/bin/dotprod-denseout -f ~/scratch-space/suitesparse/ecology1/ecology1.mtx
./build/bin/dotprod-denseout -f ~/scratch-space/suitesparse/consph/consph.mtx
./build/bin/dotprod-denseout -f ~/scratch-space/suitesparse/largebasis/largebasis.mtx
./build/bin/dotprod-denseout -f ~/scratch-space/suitesparse/shipsec1/shipsec1.mtx
./build/bin/dotprod-denseout -f ~/scratch-space/suitesparse/atmosmodd/atmosmodd.mtx
./build/bin/dotprod-denseout -f ~/scratch-space/suitesparse/pwtk/pwtk.mtx
./build/bin/dotprod-denseout -f ~/scratch-space/suitesparse/circuit5M/circuit5M.mtx

./build/bin/tensorcontract-1dout -f ./tensors/A.tns
./build/bin/tensor-elwisemul -f ./tensors/A.tns
./build/bin/singlemat-contractl -f ./tensors/bus.mtx

export LD_LIBRARY_PATH=/home/min/a/kadhitha/scratch-space/taco/build/lib:$LD_LIBRARY_PATH
./build/bin/singlemat-contractl -f ../matrices/Lin.mtx

./build/bin/sort3_manual -f ../tns/freebase_music.tns

./build/bin/tensor-elwisemul -f ../tns/vast-2015-mc1-3d.tns
./build/bin/tensor-elwisemul -f ../tns/darpa1998.tns
./build/bin/tensor-elwisemul -f ../tns/nell-2.tns
./build/bin/tensor-elwisemul -f ../tns/freebase_music.tns
./build/bin/tensor-elwisemul -f ../tns/flickr-3d.tns
./build/bin/tensor-elwisemul -f ../tns/freebase_sampled.tns
./build/bin/tensor-elwisemul -f ../tns/nell-1.tns

./build/bin/tensorcontract-1dout -f ./tensors/A.tns

./build/bin/tensorcontract-1dout -f ../tns/vast-2015-mc1-3d.tns
./build/bin/tensorcontract-1dout -f ../tns/darpa1998.tns
./build/bin/tensorcontract-1dout-f ../tns/nell-2.tns
./build/bin/tensorcontract-1dout -f ../tns/freebase_music.tns
./build/bin/tensorcontract-1dout -f ../tns/flickr-3d.tns
./build/bin/tensorcontract-1dout -f ../tns/freebase_sampled.tns
./build/bin/tensorcontract-1dout -f ../tns/nell-1.tns