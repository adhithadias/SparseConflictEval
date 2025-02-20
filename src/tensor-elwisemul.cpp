#include <random>
#include <chrono>
#include <thread>

#include "input_parser.h"
#include "iterator.h"
#include "gen_mat.h"

#define TACO_MIN(_a,_b) ((_a) < (_b) ? (_a) : (_b))

int computeexpr(taco_tensor_t *Y, taco_tensor_t *A, taco_tensor_t *B);

int computeexpr(taco_tensor_t *Y, taco_tensor_t *A, taco_tensor_t *B) {
    double* Y_vals = (double*)(Y->vals);
    int* __restrict A1_pos = (int*)(A->indices[0][0]);
    int* __restrict A1_crd = (int*)(A->indices[0][1]);
    int* __restrict A2_pos = (int*)(A->indices[1][0]);
    int* __restrict A2_crd = (int*)(A->indices[1][1]);
    int* __restrict A3_pos = (int*)(A->indices[2][0]);
    int* __restrict A3_crd = (int*)(A->indices[2][1]);
    double* __restrict A_vals = (double*)(A->vals);
    int* __restrict B1_pos = (int*)(B->indices[0][0]);
    int* __restrict B1_crd = (int*)(B->indices[0][1]);
    int* __restrict B2_pos = (int*)(B->indices[1][0]);
    int* __restrict B2_crd = (int*)(B->indices[1][1]);
    int* __restrict B3_pos = (int*)(B->indices[2][0]);
    int* __restrict B3_crd = (int*)(B->indices[2][1]);
    double* __restrict B_vals = (double*)(B->vals);

    int32_t iA = A1_pos[0];
    int32_t pA1_end = A1_pos[1];
    int32_t iB = B1_pos[0];
    int32_t pB1_end = B1_pos[1];

    int32_t jcount = 0;

    while (iA < pA1_end && iB < pB1_end) {
        int32_t iA0 = A1_crd[iA];
        int32_t iB0 = B1_crd[iB];
        int32_t i = TACO_MIN(iA0,iB0);
        if (i == iA0 && i == iB0) {
            for (int32_t jA = A2_pos[iA]; jA < A2_pos[(iA + 1)]; jA++) {
                int32_t jA0 = A2_crd[jA];
                
                int32_t kA = A3_pos[jA];
                int32_t pA3_end = A3_pos[(jA + 1)];
                int32_t kB = B2_pos[iB];
                int32_t pB2_end = B2_pos[(iB + 1)];

                while (kA < pA3_end && kB < pB2_end) {
                    int32_t kA0 = A3_crd[kA];
                    int32_t kB0 = B2_crd[kB];
                    int32_t k = TACO_MIN(kA0, kB0);
                    if (kA0 == k && kB0 == k) {
                        int32_t lowerBound = B3_pos[kB];
                        int32_t upperBound = B3_pos[(kB + 1)];

                        int32_t target = jA0;
                        if (B3_crd[lowerBound] > target) {
                            continue;
                        }

                        while (upperBound - lowerBound >= 0) {
                            int32_t mid = (upperBound + lowerBound) / 2;
                            int midValue = B3_crd[mid];
                            if (midValue < target) {
                                lowerBound = mid + 1;
                            } else if (midValue > target) {
                                upperBound = mid;
                            } else {
                                Y_vals[jcount++] = A_vals[kA] * B_vals[mid];
                                break;
                            }
                        }

                    }
                    kA += (int32_t)(kA0 == k);
                    kB += (int32_t)(kB0 == k);
                }

            }
        }
        iA += (int32_t)(iA0 == i);
        iB += (int32_t)(iB0 == i);
    }



    return 0;
}


int main(int argc, char* argv[]) {

    int nnz = 0;

    InputParser input(argc, argv);

    // std::string filename = "temp/A.tns";
    // gen_3d_tns(4, 4, 4, 0.8, filename);

    if (!input.cmdOptionExists("-f")) {
        std::cout << "No input matrix specified. Specify with -i <filename> Exiting..." << std::endl;
        return 1;
    }

    const std::string &filename = input.getCmdOption("-f");

    Format csf({Sparse, Sparse, Sparse});

    Tensor<double> A = read(filename, csf);
    A.setName("A");
    A.pack();
    taco_tensor_t* a_storage = A.getStorage();
    // nnz = iterateSSS(a_storage);
    cout << "A dimensions: " << A.getDimensions()[0] 
        << ", " << A.getDimensions()[1] << ", " << A.getDimensions()[2] << endl;
    cout << "nnz: " << nnz << endl;

    // exit(0);

    Tensor<double> B = read(filename, csf);
    B.setName("B");
    B.pack();

    std::cout << "====================" << std::endl;

    auto tic = std::chrono::high_resolution_clock::now();
    Tensor<double> AT = A.transpose({0, 2, 1}, csf);
    auto toc = std::chrono::high_resolution_clock::now();
    auto dur_transpose = std::chrono::duration_cast<std::chrono::microseconds>(toc-tic).count()/1000.0;
    AT.setName("AT");
    AT.pack();
    taco_tensor_t* at_storage = AT.getStorage();
    // iterateSSS(at_storage);
    cout << "AT dimensions: " << AT.getDimensions()[0] 
        << ", " << AT.getDimensions()[1] << ", " << AT.getDimensions()[2] << endl;

    std::cout << "====================" << std::endl;

    tic = std::chrono::high_resolution_clock::now();
    Tensor<double> ATT = AT.transpose({0, 2, 1}, csf);
    toc = std::chrono::high_resolution_clock::now();
    auto dur_transpose_back = std::chrono::duration_cast<std::chrono::microseconds>(toc-tic).count()/1000.0;
    ATT.setName("ATT");
    ATT.pack();

    IndexVar i, j, k;

    Tensor<double> Y({A.getDimension(0), A.getDimension(1), A.getDimension(2)}, csf);
    Y(i, j, k) = A(i, j, k) * ATT(i, j, k);
    Y.compile();
    Y.assemble();
    taco_tensor_t* y_storage = Y.getStorage();

    tic = std::chrono::high_resolution_clock::now();
    Y.compute();
    toc = std::chrono::high_resolution_clock::now();
    auto dur_taco = std::chrono::duration_cast<std::chrono::microseconds>(toc-tic).count()/1000.0;

    Tensor<double> Y1({A.getDimension(0), A.getDimension(1), A.getDimension(2)}, csf);
    Y1(i, j, k) = A(i, j, k) * B(i, j, k);
    Y1.compile();
    Y1.assemble();


    taco_tensor_t* y1_storage = Y1.getStorage();
    tic = std::chrono::high_resolution_clock::now();
    computeexpr(y1_storage, a_storage, at_storage);
    toc = std::chrono::high_resolution_clock::now();
    auto dur_ours = std::chrono::duration_cast<std::chrono::microseconds>(toc-tic).count()/1000.0;

    int32_t y1_dimension = (int)(y_storage->dimensions[0]);
    int* __restrict y1_pos = (int*)(y_storage->indices[1][0]);
    nnz = y1_pos[y1_dimension];
    // check_equality(nnz, (double*)y_storage->vals, (double*)y1_storage->vals);
    check_equality_sss(y_storage, y1_storage);

    std::cout << "transpose(ms), transpose back(ms), taco(ms), Fused ours(ms)\n"
        << dur_transpose 
        << ", " << dur_transpose_back
        << ", " << dur_taco
        << ", " << dur_ours
        << std::endl;

    // this_thread::sleep_for(chrono::seconds(10));

    return 0;
}