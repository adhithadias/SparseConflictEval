#include <random>
#include <chrono>
#include <thread>

#include "taco.h"
#include "input_parser.h"
#include "iterator.h"

#define NEW_PATH true

using namespace taco;
using namespace std;

bool assembleWhileCompute = true;

int main(int argc, char* argv[]) {

    // add code to get the only input matrix from the terminal with -i flag
    
    InputParser input(argc, argv);

    if (!input.cmdOptionExists("-n")) {
        std::cout << "No input matrix size specified." << std::endl;
        return 1;
    }

    if (!input.cmdOptionExists("-d")) {
        std::cout << "No input matrix density specified." << std::endl;
        return 1;
    }

    if (!input.cmdOptionExists("-r")) {
        std::cout << "No input matrix repetitions specified." << std::endl;
        return 1;
    }

    const int n = std::stoi(input.getCmdOption("-n"));
    const double density = std::stod(input.getCmdOption("-d"));
    const int repetitions = std::stoi(input.getCmdOption("-r"));
    const string file = input.getCmdOption("-f");
    std::default_random_engine gen(1000);
    std::uniform_real_distribution<double> unif(0.0, 1.0);

    Format csr({Dense,Sparse});

    Tensor<double> A({n, n}, csr);
    A.setName("A");

    Tensor<double> B({n, n}, csr);
    B.setName("B");

    Tensor<double> C({n, n}, csr);
    C.setName("C");

    Tensor<double> D({n, n}, csr);
    D.setName("D");

    // fill B and C with random values based on density
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (unif(gen) < density) {
                B.insert({i, j}, unif(gen));
            }
            if (unif(gen) < density) {
                C.insert({i, j}, unif(gen));
            }
            if (unif(gen) < density) {
                C.insert({i, j}, unif(gen));
            }
        }
    }

    B.pack();
    C.pack();
    D.pack();

    // print B matrix
    

    IndexVar i("i"), j("j"), k("k");
    A(i,j) = B(i,k) * C(k,j) * D(k,j);

    IndexStmt stmt = A.getAssignment().concretize(NEW_PATH);
    A.setNewPath(NEW_PATH);
    A.setAssembleWhileCompute(assembleWhileCompute);
    A.compile(stmt, assembleWhileCompute); // IndexStmt stmt, bool assembleWhileCompute
    if (!assembleWhileCompute) {
        A.assemble();
    }
    std::cout << "compiled: " << stmt << std::endl;

    std::vector<std::chrono::microseconds> times;

    A.compute();

    for (int r = 1; r < repetitions; r++) {
        auto start = std::chrono::high_resolution_clock::now();
        A.compute();
        auto end = std::chrono::high_resolution_clock::now();
        times.push_back(
            std::chrono::duration_cast<std::chrono::microseconds>(end-start));
    }

    // annonymous function to find median
    auto find_median = [](std::vector<std::chrono::microseconds> &v) {
        std::sort(v.begin(), v.end(), std::less<std::chrono::microseconds>());
        return v[v.size()/2].count() / 1000.0;
    };

    // // find median from time_to_compute_fuse
    double fused_median = find_median(times);
    cout << "Fused median: " << fused_median << " ms" << endl;

    // return 0;
    
    Tensor<double> E({n, n}, csr);
    E.setName("E");

    E(k,j) = C(k,j) * D(k,j);
    E.setAssembleWhileCompute(assembleWhileCompute);
    E.compile();
    if (!assembleWhileCompute) {
        E.assemble();
    }

    A(i,j) = B(i,k) * E(k,j);
    A.setAssembleWhileCompute(assembleWhileCompute);
    A.compile();
    if (!assembleWhileCompute) {
        A.assemble();
    }

    E.compute();
    // A.compute();

    std::vector<std::chrono::microseconds> times_taco1;
    std::vector<std::chrono::microseconds> times_taco2;
    std::vector<std::chrono::microseconds> times_taco;
    for (int r = 0; r < repetitions; r++) {
        auto start = std::chrono::high_resolution_clock::now();
        E.compute();
        auto end = std::chrono::high_resolution_clock::now();
        A.compute();
        auto end2 = std::chrono::high_resolution_clock::now();
        times_taco1.push_back(
            std::chrono::duration_cast<std::chrono::microseconds>(end-start));
        times_taco2.push_back(
            std::chrono::duration_cast<std::chrono::microseconds>(end2-end));
        times_taco.push_back(
            std::chrono::duration_cast<std::chrono::microseconds>(end2-start));
    }


    double taco_median = find_median(times_taco);
    cout << "Taco median: " << taco_median << " ms" << endl;

    // print taco_times1, taco_times2, taco_times 
    cout << "hadamard: ";
    for (size_t i = 0; i < times_taco1.size(); i++) {
        cout << times_taco1[i].count() / 1000.0 << ", ";
    }
    cout << endl << "matmul: ";
    for (size_t i = 0; i < times_taco2.size(); i++) {
        cout << times_taco2[i].count() / 1000.0 << ", ";
    }
    cout << endl << "Taco total time: ";
    for (size_t i = 0; i < times_taco.size(); i++) {
        cout << times_taco[i].count() / 1000.0 << ", ";
    }
    cout << endl;

    double hadamard_median = 0;
    double matmul_median = 0;
    double gap = std::numeric_limits<double>::max();
    for (size_t i = 0; i < times.size(); i++) {
        auto sum = (times_taco1[i] + times_taco2[i]).count() / 1000.0;
        // take absolute difference of sum and taco_median
        if (abs(sum - taco_median) < gap) {
            hadamard_median = times_taco1[i].count() / 1000.0;
            matmul_median = times_taco2[i].count() / 1000.0;
            gap = abs(sum - taco_median);
        }
    }

    std::cout << "Size, Density, Ours (ms), Hadamard (ms), Matmul (ms), Taco (ms)" << std::endl;
    std::cout << n << ", " << density << ", " << fused_median << ", " << hadamard_median << ", " 
        << matmul_median << ", " << taco_median << std::endl;
    
    return 0;
}