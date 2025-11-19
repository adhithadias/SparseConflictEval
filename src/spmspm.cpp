#include <random>
#include <chrono>
#include <thread>

#include "taco.h"
#include "input_parser.h"
#include "iterator.h"

#define NEW_PATH false

using namespace taco;
using namespace std;

int computeexpr(taco_tensor_t *Y, taco_tensor_t *A, taco_tensor_t *B) {
    int A1_dimension = (int)(A->dimensions[0]);
    int* __restrict A2_pos = (int*)(A->indices[1][0]);
    int* __restrict A2_crd = (int*)(A->indices[1][1]);
    double* __restrict A_vals = (double*)(A->vals);

    int B1_dimension = (int)(B->dimensions[0]);
    int* __restrict B2_pos = (int*)(B->indices[1][0]);
    int* __restrict B2_crd = (int*)(B->indices[1][1]);
    double* __restrict B_vals = (double*)(B->vals);

    int Y1_dimension = (int)(Y->dimensions[0]);
    double* Y_vals = (double*)(Y->vals);

    int32_t jcount = 0;

    for (int32_t i = 0; i < A1_dimension; i++) {

        for (int32_t jA = A2_pos[i]; jA < A2_pos[i+1]; jA++) {
            int32_t j = A2_crd[jA];
            double A_ij = A_vals[jA];

            int32_t lowerBound = B2_pos[j];
            int32_t upperBound = B2_pos[j+1];

            // Assume B2_Crd is sorted
            // Search in B2_Crd array using BinarySearch
            int32_t target = i; 
            if (B2_crd[lowerBound] > target) {
                continue; // early exit
            }

            while (upperBound - lowerBound >= 0) {
                int32_t mid = (upperBound + lowerBound) / 2;
                int midValue = B2_crd[mid];
                if (midValue < target) {
                    lowerBound = mid + 1;
                } else if (midValue > target) {
                    upperBound = mid - 1;
                } else {
                    Y_vals[jcount++] = A_ij * B_vals[mid];
                    break;
                }
            }
        }
    }

    return 0;
}

int main(int argc, char* argv[]) {

    // add code to get the only input matrix from the terminal with -i flag
    
    InputParser input(argc, argv);

    if (!input.cmdOptionExists("-f")) {
        std::cout << "No input matrix specified. Specify with -f <filename> Exiting..." << std::endl;
        return 1;
    }

    const std::string &filename = input.getCmdOption("-f");

    std::default_random_engine gen(0);
    std::uniform_real_distribution<double> unif(0.0, 1.0);

    Format csr({Dense,Sparse});
    Format ss({Sparse,Sparse});

    // std::string file_name = "bus.mtx";
    // std::string file_name = "Trec5.mtx";
    // std::string file_name = "cant.mtx";
    // std::string file_name = "circuit5M.mtx";

    Tensor<double> A = read(filename, csr);
    A.setName("A");
    std::cout << "A dimensions: " << A.getDimensions()[0] 
        << ", " << A.getDimensions()[1] << std::endl;
    A.pack();

    Tensor<double> C = read(filename, ss);
    C.setName("C");
    C.pack();

    auto tic = std::chrono::high_resolution_clock::now();
    Tensor<double> B = A.transpose({1, 0}, csr);
    auto toc = std::chrono::high_resolution_clock::now();

    std::vector<std::chrono::microseconds> time_to_transpose;

    // for (int i = 0; i < 5; i++) {
    //     tic = std::chrono::high_resolution_clock::now();
    //     B = A.transpose({1, 0}, csr);
    //     toc = std::chrono::high_resolution_clock::now();
    //     time_to_transpose.push_back(std::chrono::duration_cast<std::chrono::microseconds>(toc-tic));
    //     cout << "Time: " << (std::chrono::duration_cast<std::chrono::microseconds>(toc-tic).count() / 1000.0) << " ms" << endl;
    // }


    B.setName("B");
    std::cout << "B dimensions: " << B.getDimensions()[0]
        << ", " << B.getDimensions()[1] << std::endl;
    B.pack();

    // std::cout << "A tensor: \n" << A << std::endl;
    // std::cout << "B tensor: \n" << B << std::endl;
    // std::cout << "C tensor: \n" << C << std::endl;

    taco_tensor_t* a_storage = A.getStorage();
    taco_tensor_t* b_storage = B.getStorage();

    // iterate(a_storage);
    cout << "====================" << endl;
    // iterate(b_storage);


    // Declare the output matrix to be a sparse matrix with the same dimensions as 
    // input matrix B, to be also stored as a doubly compressed sparse row matrix.
    Tensor<double> Y({A.getDimension(0), B.getDimension(1)}, csr);
    Y.setName("Y");

    // Define the SpMV computation using index notation.
    IndexVar i("i"), j("j"), k("k");

    Y(i,j) = A(i,k) * B(k,j);

    IndexStmt stmt = Y.getAssignment().concretize(NEW_PATH);

    Y.setNewPath(NEW_PATH);
    Y.setAssembleWhileCompute(true);
    Y.compile();
    std::cout << "compiled: " << stmt << std::endl;
    // Y.assemble();

    std::vector<std::chrono::microseconds> time_to_compute_fuse;

    Y.compute();
    for (int z = 0; z < 5; z++) {
        tic = std::chrono::high_resolution_clock::now();
        Y.compute();
        toc = std::chrono::high_resolution_clock::now();
        time_to_compute_fuse.push_back(
            std::chrono::duration_cast<std::chrono::microseconds>(toc-tic));
        // cout << "Time: " << (time_to_compute_fuse[z].count() / 1000.0) << " ms" << endl;
    }

    // add wait of 5 seconds here
    // std::this_thread::sleep_for(std::chrono::seconds(5));

    // time_to_compute_fuse = std::chrono::duration_cast<std::chrono::microseconds>(toc-tic);

    // Tensor<double> Y1({A.getDimension(0), C.getDimension(0)}, csr);
    // Y1.setName("Y1");
    // Y1.setAssembleWhileCompute(true);
    // Y1(i,j) = A(i,k) * C(j,k);
    // IndexStmt stmt1 = Y1.getAssignment().concretize();

    // Y1.compile();
    // std::cout << "compiled: " << stmt1 << std::endl;
    // Y1.assemble();

    // std::vector<std::chrono::microseconds> time_to_compute_taco;

    // Y1.compute();
    // for (int z = 0; z < 5; z++) {
    //     tic = std::chrono::high_resolution_clock::now();
    //     Y1.compute();    
    //     toc = std::chrono::high_resolution_clock::now();
    //     time_to_compute_taco.push_back(
    //         std::chrono::duration_cast<std::chrono::microseconds>(toc-tic));
    //     cout << "Time: " << time_to_compute_taco[z].count() / 1000.0 << " ms" << endl;
    // }

    // taco_tensor_t* y_storage = Y.getStorage();
    // // y_storage = y.getStorage();
    // double* y_vals = (double*)(y_storage->vals);

    // taco_tensor_t* y1_storage = Y1.getStorage();
    // double* y1_vals = (double*)(y1_storage->vals);

    // // double* y_vals = (double*)(y_storage->vals);
    // int32_t y1_dimension = (int)(y_storage->dimensions[0]);
    // int* __restrict y1_pos = (int*)(y_storage->indices[1][0]);
    // int32_t nnz = y1_pos[y1_dimension];


    // // check if the output tensors are equal
    // bool equal = true;
    // int count = 0;
    // for (int i = 0; i < nnz; i++) {
    //     if (y_vals[i] - y1_vals[i] > 0.01 ) {
    //         equal = false;
    //         cout << "Error: " << i << ": " << y_vals[i] << " != " << y1_vals[i] <<  endl;
    //         break;
    //     } else {
    //         count++;
    //     }
    // }
    // cout << "Equal: " << count << endl;

    // std::cout << "Y tensor: \n" << Y << std::endl;
    // std::cout << "Y1 tensor: \n" << Y1 << std::endl;

    // annonymous function to find median
    auto find_median = [](std::vector<std::chrono::microseconds> &v) {
        std::sort(v.begin(), v.end(), std::less<std::chrono::microseconds>());
        return v[v.size()/2].count() / 1000.0;
    };

    // // find median from time_to_compute_fuse
    double fused_median = find_median(time_to_compute_fuse);
    // double taco_median = find_median(time_to_compute_taco);
    // double transpose_median = find_median(time_to_transpose);

    cout << "Fused median: " << fused_median << " ms" << endl;
    // cout << "Taco Transpose (ms), Taco (ms), Fused(Ours) (ms)" << endl;
    // cout << transpose_median << ", " << taco_median << ", " 
    //     << fused_median << endl;
    
    // confirm y_copy_

    // Write the output of the computation to file (stored in the FROSTT format).
    // write("y.tns", y);
}