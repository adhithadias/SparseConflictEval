#include <random>
#include <chrono>
#include "taco.h"
#include "iterator.h"
#include "input_parser.h"


using namespace taco;
using namespace std;


int computeexpr(taco_tensor_t *y, taco_tensor_t *A, taco_tensor_t *B) {
    int A1_dimension = (int)(A->dimensions[0]);
    int* __restrict A2_pos = (int*)(A->indices[1][0]);
    int* __restrict A2_crd = (int*)(A->indices[1][1]);
    double* __restrict A_vals = (double*)(A->vals);

    int B1_dimension = (int)(B->dimensions[0]);
    int* __restrict B2_pos = (int*)(B->indices[1][0]);
    int* __restrict B2_crd = (int*)(B->indices[1][1]);
    double* __restrict B_vals = (double*)(B->vals);

    int y1_dimension = (int)(y->dimensions[0]);
    double* y_vals = (double*)(y->vals);

    int32_t ia = 0;

    for (int32_t i = 0; i < A1_dimension; i++) {
        double tyval = 0;

        for (int32_t jA = A2_pos[i]; jA < A2_pos[i+1]; jA++) {
            int32_t j = A2_crd[jA];
            double A_ij = A_vals[jA];

            int32_t lowerBound = B2_pos[j];
            int32_t upperBound = B2_pos[j+1];

            if (upperBound - lowerBound > 5) {
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
                        tyval += A_ij * B_vals[mid];
                        break;
                    }
                }

            } else {

                for (int32_t jB = lowerBound; jB < upperBound; jB++) {
                    int32_t l = B2_crd[jB];
                    double B_kl = B_vals[jB];
                    if (l == i) {
                        tyval += A_ij * B_kl;
                        break;
                    }
                }

            }
        }
        y_vals[ia] = tyval;
        ia++;
    }

    return 0;
}


int main(int argc, char* argv[]) {

    // add code to get the only input matrix from the terminal with -i flag
    
    InputParser input(argc, argv);

    if (!input.cmdOptionExists("-f")) {
        std::cout << "No input matrix specified. Specify with -i <filename> Exiting..." << std::endl;
        return 1;
    }

    const std::string &filename = input.getCmdOption("-f");

    std::default_random_engine gen(0);
    std::uniform_real_distribution<double> unif(0.0, 1.0);

    Format csr({Dense, Sparse});
    Format spv({Sparse});

    // std::string file_name = "bus.mtx";
    // std::string file_name = "Trec5.mtx";
    // std::string file_name = "cant.mtx";
    // std::string file_name = "circuit5M.mtx";

    Tensor<double> A = read(filename, csr);
    A.setName("A");
    std::cout << "A dimensions: " << A.getDimensions()[0] 
        << ", " << A.getDimensions()[1] << std::endl;
    A.pack();

    Tensor<double> C = read(filename, csr);
    C.setName("C");
    C.pack();

    auto tic = std::chrono::high_resolution_clock::now();
    Tensor<double> B = A.transpose({1, 0}, csr);
    auto toc = std::chrono::high_resolution_clock::now();

    std::vector<std::chrono::microseconds> time_to_transpose;

    for (int i = 0; i < 5; i++) {
        tic = std::chrono::high_resolution_clock::now();
        B = A.transpose({1, 0}, csr);
        toc = std::chrono::high_resolution_clock::now();
        time_to_transpose.push_back(std::chrono::duration_cast<std::chrono::microseconds>(toc-tic));
        cout << "Time: " << (std::chrono::duration_cast<std::chrono::microseconds>(toc-tic).count() / 1000.0) << " ms" << endl;
    }


    B.setName("B");
    std::cout << "B dimensions: " << B.getDimensions()[0]
        << ", " << B.getDimensions()[1] << std::endl;
    B.pack();

    taco_tensor_t* a_storage = A.getStorage();
    taco_tensor_t* b_storage = B.getStorage();

    // iterate_csr(a_storage);
    cout << "====================" << endl;
    // iterate_csr(b_storage);


    // Declare the output matrix to be a sparse matrix with the same dimensions as 
    // input matrix B, to be also stored as a doubly compressed sparse row matrix.
    Tensor<double> y({A.getDimension(0)}, spv);
    taco_tensor_t* y_storage = y.getStorage();

    // Define the SpMV computation using index notation.
    IndexVar i, j;

    y(i) = A(i,j) * C(i,j);
    y.compile();
    y.assemble();

    std::vector<std::chrono::microseconds> time_to_compute_fuse;

    computeexpr(y_storage, a_storage, b_storage);
    for (int z = 0; z < 5; z++) {
        tic = std::chrono::high_resolution_clock::now();
        computeexpr(y_storage, a_storage, b_storage);
        toc = std::chrono::high_resolution_clock::now();
        time_to_compute_fuse.push_back(
            std::chrono::duration_cast<std::chrono::microseconds>(toc-tic));
        cout << "Time: " << (time_to_compute_fuse[z].count() / 1000.0) << " ms" << endl;
    }
    // time_to_compute_fuse = std::chrono::duration_cast<std::chrono::microseconds>(toc-tic);


    double* y_vals = (double*)(y_storage->vals);

    // malloc double array to copy values of size y.getDimension(0)
    double* y_copy_vals = (double*)malloc(y.getDimension(0) * sizeof(double));

    // copy the values from y to y_copy
    for (int i = 0; i < y.getDimension(0); i++) {
        y_copy_vals[i] = y_vals[i];
    }

    std::vector<std::chrono::microseconds> time_to_compute_taco;

    y.compute();
    for (int z = 0; z < 5; z++) {
        tic = std::chrono::high_resolution_clock::now();
        y.compute();
        toc = std::chrono::high_resolution_clock::now();
        time_to_compute_taco.push_back(
            std::chrono::duration_cast<std::chrono::microseconds>(toc-tic));
        cout << "Time: " << time_to_compute_taco[z].count() / 1000.0 << " ms" << endl;
    }

    y_storage = y.getStorage();
    y_vals = (double*)(y_storage->vals);

    // check if the output tensors are equal
    bool equal = true;
    int count = 0;
    for (int i = 0; i < A.getDimension(0); i++) {
        if (y_vals[i] - y_copy_vals[i] > 0.01 ) {
            equal = false;
            cout << "Error: " << i << ": " << y_vals[i] << " != " << y_copy_vals[i] <<  endl;
            break;
        } else {
            count++;
        }
    }
    cout << "Equal: " << count << endl;

    // annonymous function to find median
    auto find_median = [](std::vector<std::chrono::microseconds> &v) {
        std::sort(v.begin(), v.end(), std::less<std::chrono::microseconds>());
        return v[v.size()/2].count() / 1000.0;
    };

    // find median from time_to_compute_fuse
    double fused_median = find_median(time_to_compute_fuse);
    double taco_median = find_median(time_to_compute_taco);
    double transpose_median = find_median(time_to_transpose);

    cout << "Taco Transpose (ms), Taco (ms), Fused(Ours) (ms)" << endl;
    cout << transpose_median << ", " << taco_median << ", " 
        << fused_median << endl;


    // confirm y_copy_

    // Write the output of the computation to file (stored in the FROSTT format).
    // write("y.tns", y);
}