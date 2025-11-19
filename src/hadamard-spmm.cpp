#include <random>
#include <chrono>
#include <thread>

#include "taco.h"
#include "input_parser.h"
#include "iterator.h"

using namespace taco;
using namespace std;

bool assembleWhileCompute = true;
bool NEW_PATH = true;

int main(int argc, char* argv[]) {

    // add code to get the only input matrix from the terminal with -i flag
    
    InputParser input(argc, argv);

    if (!input.cmdOptionExists("-f")) {
        std::cout << "No input matrix file specified." << std::endl;
        return 1;
    }

    if (!input.cmdOptionExists("-r")) {
        std::cout << "No repetitions specified." << std::endl;
        return 1;
    }

    const string file = input.getCmdOption("-f");
    const int repetitions = input.cmdOptionExists("-r") ? std::stoi(input.getCmdOption("-r")) : 5;
    std::default_random_engine gen(1000);
    std::uniform_real_distribution<double> unif(0.0, 1.0);

    Format csr({Dense,Sparse});

    // Tensor<double> B({n, n}, csr);
    // B.setName("B");
    Tensor<double> B = read(file, csr);
    B.setName("B");

    // Tensor<double> C({n, n}, csr);
    // C.setName("C");
    Tensor<double> C = read(file, csr);
    C.setName("C");

    // Tensor<double> D({n, n}, csr);
    // D.setName("D");
    Tensor<double> D = read(file, csr);
    D.setName("D");

    // // fill B and C with random values based on density
    // for (int i = 0; i < n; i++) {
    //     for (int j = 0; j < n; j++) {
    //         if (unif(gen) < density) {
    //             B.insert({i, j}, unif(gen));
    //         }
    //         if (unif(gen) < density) {
    //             C.insert({i, j}, unif(gen));
    //         }
    //         if (unif(gen) < density) {
    //             C.insert({i, j}, unif(gen));
    //         }
    //     }
    // }

    B.pack();
    C.pack();
    D.pack();

    Tensor<double> G = D.transpose({1,0}, csr);
    G.setName("G");
    G.pack();

    Tensor<double> A({B.getDimension(0), C.getDimension(1)}, csr);
    A.setName("A");

    // print B matrix
    

    IndexVar i("i"), j("j"), k("k");
    A(i,j) = B(i,k) * C(k,j) * G(k,j);

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
    
    Tensor<double> E({C.getDimension(0), C.getDimension(1)}, csr);
    E.setName("E");

    E(k,j) = C(k,j) * G(k,j);
    E.setAssembleWhileCompute(assembleWhileCompute);
    E.compile();
    if (!assembleWhileCompute) {
        E.assemble();
    }

    Tensor<double> F({B.getDimension(0), C.getDimension(1)}, csr);
    F.setName("F");

    F(i,j) = B(i,k) * E(k,j);
    F.setAssembleWhileCompute(assembleWhileCompute);
    F.compile();
    if (!assembleWhileCompute) {
        F.assemble();
    }

    E.compute();
    F.compute();
    std::vector<std::chrono::microseconds> times_taco1;
    std::vector<std::chrono::microseconds> times_taco2;
    std::vector<std::chrono::microseconds> times_taco;
    for (int r = 0; r < repetitions; r++) {
        auto start = std::chrono::high_resolution_clock::now();
        E.compute();
        auto end = std::chrono::high_resolution_clock::now();
        F.compute();
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
    
    return 0;
}