#include <random>
#include <chrono>

#include "taco.h"
#include "input_parser.h"
#include "iterator.h"

using namespace taco;
using namespace std;

bool assembleWhileCompute = true;

inline __attribute__((always_inline)) int index_search(int *array, int arrayStart, int arrayEnd, int target) {
    if (array[arrayStart] > target || ((arrayEnd - arrayStart > 0) && array[arrayEnd-1] < target)) {
      return -1; // early exit
    }
    if (arrayEnd - arrayStart > 5) {
    while (arrayEnd - arrayStart >= 0) {
      int mid = (arrayEnd + arrayStart) / 2;
      int midValue = array[mid];
      if (midValue < target) {
        arrayStart = mid + 1;
      }
      else if (midValue > target) {
        arrayEnd = mid - 1;
      }
      else {
        return mid;
      }
    }
    }
    else {
      for (int32_t idx = arrayStart; idx < arrayEnd; idx++) {
        int32_t column_idx = array[idx];
        if (column_idx == target) {
          return idx;
        }
      }
    }
    return -1;
}

int computeexpr(taco_tensor_t *Y, taco_tensor_t *B, taco_tensor_t *C) {
    int B1_dimension = (int)(B->dimensions[0]);
    int* __restrict B2_pos = (int*)(B->indices[1][0]);
    int* __restrict B2_crd = (int*)(B->indices[1][1]);
    double* __restrict B_vals = (double*)(B->vals);

    int C1_dimension = (int)(C->dimensions[0]);
    int* __restrict C2_pos = (int*)(C->indices[1][0]);
    int* __restrict C2_crd = (int*)(C->indices[1][1]);
    double* __restrict C_vals = (double*)(C->vals);

    int Y1_dimension = (int)(Y->dimensions[0]);
    double* Y_vals = (double*)(Y->vals);

    int32_t jcount = 0;

    // A(i,j) = B(i,j) * C(j,i)
    for (int32_t i = 0; i < B1_dimension; i++) {

        for (int32_t jB = B2_pos[i]; jB < B2_pos[i+1]; jB++) {
            int32_t j = B2_crd[jB];
            double B_ij = B_vals[jB];

            int32_t lowerBound = C2_pos[j];
            int32_t upperBound = C2_pos[j+1];

            // Assume B2_Crd is sorted
            // Search in B2_Crd array using BinarySearch
            int32_t target = i; 
            if (C2_crd[lowerBound] > target) {
                continue; // early exit
            }

            while (upperBound - lowerBound >= 0) {
                int32_t mid = (upperBound + lowerBound) / 2;
                int midValue = C2_crd[mid];
                if (midValue < target) {
                    lowerBound = mid + 1;
                } else if (midValue > target) {
                    upperBound = mid - 1;
                } else {
                    Y_vals[jcount] = B_ij * C_vals[mid];
                    jcount++;
                    break;
                }
            }
        }
    }

    return 0;
}

int compute_only_expr(taco_tensor_t *y, taco_tensor_t *A, taco_tensor_t *C) {
    int y1_dimension = (int)(y->dimensions[0]);
    int* __restrict y2_pos = (int*)(y->indices[1][0]);
    int* __restrict y2_crd = (int*)(y->indices[1][1]);
    double* __restrict y_vals = (double*)(y->vals);
    int A1_dimension = (int)(A->dimensions[0]);
    int* __restrict A2_pos = (int*)(A->indices[1][0]);
    int* __restrict A2_crd = (int*)(A->indices[1][1]);
    double* __restrict A_vals = (double*)(A->vals);
    int C1_dimension = (int)(C->dimensions[0]);
    int C2_dimension = (int)(C->dimensions[1]);
    int* __restrict C2_pos = (int*)(C->indices[1][0]);
    int* __restrict C2_crd = (int*)(C->indices[1][1]);
    double* __restrict C_vals = (double*)(C->vals);

    int32_t jy = 0;
  
    for (int32_t i = 0; i < C2_dimension; i++) {
  
      for (int32_t jA = A2_pos[i]; jA < A2_pos[(i + 1)]; jA++) {
        int32_t j = A2_crd[jA];

        int32_t index_found = index_search(C2_crd, C2_pos[j], C2_pos[(j + 1)], i);
        if (index_found != -1) {
          /* Search node */
          y_vals[jy] = A_vals[jA] * C_vals[index_found];
          jy++;
        }
      }
    }
    return 0;
}

int compute_and_assemble_expr(taco_tensor_t *y, taco_tensor_t *A, taco_tensor_t *C) {
    int y1_dimension = (int)(y->dimensions[0]);
    int* __restrict y2_pos = (int*)(y->indices[1][0]);
    int* __restrict y2_crd = (int*)(y->indices[1][1]);
    double* __restrict y_vals = (double*)(y->vals);
    int A1_dimension = (int)(A->dimensions[0]);
    int* __restrict A2_pos = (int*)(A->indices[1][0]);
    int* __restrict A2_crd = (int*)(A->indices[1][1]);
    double* __restrict A_vals = (double*)(A->vals);
    int C1_dimension = (int)(C->dimensions[0]);
    int C2_dimension = (int)(C->dimensions[1]);
    int* __restrict C2_pos = (int*)(C->indices[1][0]);
    int* __restrict C2_crd = (int*)(C->indices[1][1]);
    double* __restrict C_vals = (double*)(C->vals);
  
    y2_pos = (int32_t*)malloc(sizeof(int32_t) * (y1_dimension + 1));
    y2_pos[0] = 0;
    for (int32_t py2 = 1; py2 < (y1_dimension + 1); py2++) {
      y2_pos[py2] = 0;
    }
    int32_t y2_crd_size = 1048576;
    y2_crd = (int32_t*)malloc(sizeof(int32_t) * y2_crd_size);
    int32_t jy = 0;
    int32_t y_capacity = 1048576;
    y_vals = (double*)malloc(sizeof(double) * y_capacity);
  
    for (int32_t i = 0; i < C2_dimension; i++) {
      int32_t py2_begin = jy;
  
      for (int32_t jA = A2_pos[i]; jA < A2_pos[(i + 1)]; jA++) {
        int32_t j = A2_crd[jA];

        int32_t index_found = index_search(C2_crd, C2_pos[j], C2_pos[(j + 1)], i);
        if (index_found != -1) {
          if (y_capacity <= jy) {
              y_vals = (double*)realloc(y_vals, sizeof(double) * (y_capacity * 2));
              y_capacity *= 2;
            }
          /* Search node */
          y_vals[jy] = A_vals[jA] * C_vals[index_found];
          if (y2_crd_size <= jy) {
            y2_crd = (int32_t*)realloc(y2_crd, sizeof(int32_t) * (y2_crd_size * 2));
            y2_crd_size *= 2;
          }
          y2_crd[jy] = j;
          jy++;
        }
      }
  
      y2_pos[i + 1] = jy - py2_begin;
    }
  
    int32_t csy2 = 0;
    for (int32_t py20 = 1; py20 < (y1_dimension + 1); py20++) {
      csy2 += y2_pos[py20];
      y2_pos[py20] = csy2;
    }
  
    y->indices[1][0] = (uint8_t*)(y2_pos);
    y->indices[1][1] = (uint8_t*)(y2_crd);
    y->vals = (uint8_t*)y_vals;
    return 0;
}

int assembleexpr(taco_tensor_t *y, taco_tensor_t *A, taco_tensor_t *C) {
    int y1_dimension = (int)(y->dimensions[0]);
    int* __restrict y2_pos = (int*)(y->indices[1][0]);
    int* __restrict y2_crd = (int*)(y->indices[1][1]);
    double* __restrict y_vals = (double*)(y->vals);
    int A1_dimension = (int)(A->dimensions[0]);
    int* __restrict A2_pos = (int*)(A->indices[1][0]);
    int* __restrict A2_crd = (int*)(A->indices[1][1]);
    double* __restrict A_vals = (double*)(A->vals);
    int C1_dimension = (int)(C->dimensions[0]);
    int C2_dimension = (int)(C->dimensions[1]);
    int* __restrict C2_pos = (int*)(C->indices[1][0]);
    int* __restrict C2_crd = (int*)(C->indices[1][1]);
    double* __restrict C_vals = (double*)(C->vals);
  
    y2_pos = (int32_t*)malloc(sizeof(int32_t) * (y1_dimension + 1));
    y2_pos[0] = 0;
    for (int32_t py2 = 1; py2 < (y1_dimension + 1); py2++) {
      y2_pos[py2] = 0;
    }
    int32_t y2_crd_size = 1048576;
    y2_crd = (int32_t*)malloc(sizeof(int32_t) * y2_crd_size);
    int32_t jy = 0;
  
    for (int32_t i = 0; i < C2_dimension; i++) {
      int32_t py2_begin = jy;
  
      for (int32_t jA = A2_pos[i]; jA < A2_pos[(i + 1)]; jA++) {
        int32_t j = A2_crd[jA];

        int32_t index_found = index_search(C2_crd, C2_pos[j], C2_pos[(j + 1)], i);
        if (index_found != -1) {
          if (y2_crd_size <= jy) {
            y2_crd = (int32_t*)realloc(y2_crd, sizeof(int32_t) * (y2_crd_size * 2));
            y2_crd_size *= 2;
          }
          y2_crd[jy] = j;
          jy++;
        }
      }
  
      y2_pos[i + 1] = jy - py2_begin;
    }
  
    int32_t csy2 = 0;
    for (int32_t py20 = 1; py20 < (y1_dimension + 1); py20++) {
      csy2 += y2_pos[py20];
      y2_pos[py20] = csy2;
    }

    y_vals = (double*)malloc(sizeof(double) * jy);
  
    y->indices[1][0] = (uint8_t*)(y2_pos);
    y->indices[1][1] = (uint8_t*)(y2_crd);
    y->vals = (uint8_t*)y_vals;
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

    Format csr({Dense,Sparse});

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

    for (int i = 0; i < 1; i++) {
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
    taco_tensor_t* c_storage = C.getStorage();

    // iterate(a_storage);
    cout << "====================" << endl;
    // iterate(b_storage);


    // Declare the output matrix to be a sparse matrix with the same dimensions as 
    // input matrix B, to be also stored as a doubly compressed sparse row matrix.
    Tensor<double> y({A.getDimension(0), A.getDimension(1)}, csr);
    y.setName("y");

    taco_tensor_t* y_storage = y.getStorage();

    // Define the SpMV computation using index notation.
    IndexVar i("i"), j("j");

    y(i,j) = A(i,j) * C(j,i);

    IndexStmt stmt = y.getAssignment().concretize(true);
    // IndexStmt stmt = y.getAssignment().concretize(true);
    std::cout << "stmt: " << stmt << std::endl;
    y.setAssembleWhileCompute(assembleWhileCompute);
    y.setNewPath(true);
    y.compile(stmt, assembleWhileCompute);
    std::cout << "After compile" << std::endl;
    if (!assembleWhileCompute) {
        // y.assemble();
        assembleexpr(y_storage, a_storage, c_storage);
    }
    std::cout << "After assemble" << std::endl;

    std::vector<std::chrono::microseconds> time_to_compute_fuse;

    // computeexpr(y_storage, a_storage, b_storage);
    if (assembleWhileCompute) {
        compute_and_assemble_expr(y_storage, a_storage, c_storage);
    } else {
        compute_only_expr(y_storage, a_storage, c_storage);
        // y.compute();
    }
    std::cout << "After compute" << std::endl;
    // compute_and_assemble_expr(y_storage, a_storage, c_storage);
    for (int z = 0; z < 5; z++) {
        if (assembleWhileCompute) {
            tic = std::chrono::high_resolution_clock::now();
            compute_and_assemble_expr(y_storage, a_storage, c_storage);
            toc = std::chrono::high_resolution_clock::now();
        } else {
            tic = std::chrono::high_resolution_clock::now();
            compute_only_expr(y_storage, a_storage, c_storage);
            // y.compute();
            toc = std::chrono::high_resolution_clock::now();
        }

        time_to_compute_fuse.push_back(
            std::chrono::duration_cast<std::chrono::microseconds>(toc-tic));
        cout << "Time: " << (time_to_compute_fuse[z].count() / 1000.0) << " ms" << endl;
    }
    // time_to_compute_fuse = std::chrono::duration_cast<std::chrono::microseconds>(toc-tic);


    double* y_vals = (double*)(y_storage->vals);
    int32_t y1_dimension = (int)(y_storage->dimensions[0]);
    int* __restrict y1_pos = (int*)(y_storage->indices[1][0]);
    int32_t nnz = y1_pos[y1_dimension];

    std::cout << "y1_dimension: " << y1_dimension << std::endl;
    std::cout << "nnz: " << nnz << std::endl;

    // malloc double array to copy values of size y.getDimension(0)
    double* y_copy_vals = (double*)malloc(nnz * sizeof(double));

    // copy the values from y to y_copy
    for (int i = 0; i < nnz; i++) {
        y_copy_vals[i] = y_vals[i];
    }

    Tensor<double> y0({A.getDimension(0), A.getDimension(1)}, csr);
    y0.setName("y");

    y0(i,j) = A(i,j) * B(i,j);

    IndexStmt stmt2 = y0.getAssignment().concretize();
    // IndexStmt stmt = y.getAssignment().concretize(true);
    std::cout << "stmt2: " << stmt2 << std::endl;
    y0.setAssembleWhileCompute(assembleWhileCompute);
    y0.compile();
    if (!assembleWhileCompute) {
        y0.assemble();
    }

    std::vector<std::chrono::microseconds> time_to_compute_taco;

    y0.compute();
    for (int z = 0; z < 5; z++) {
        tic = std::chrono::high_resolution_clock::now();
        y0.compute();        
        toc = std::chrono::high_resolution_clock::now();
        time_to_compute_taco.push_back(
            std::chrono::duration_cast<std::chrono::microseconds>(toc-tic));
        cout << "Time: " << time_to_compute_taco[z].count() / 1000.0 << " ms" << endl;
    }

    taco_tensor_t* y0_storage = y0.getStorage();
    // std::cout << "y0_storage: " << std::endl;
    double* y0_vals = (double*)(y0_storage->vals);
    int32_t y1_dimension_ = (int)(y0_storage->dimensions[0]);
    int* __restrict y1_pos_ = (int*)(y0_storage->indices[1][0]);
    int32_t nnz_ = y1_pos_[y1_dimension];
    // std::cout << "y1_dimension_: " << y1_dimension_ << std::endl;
    // std::cout << "nnz: " << nnz_ << std::endl;

    // check if the output tensors are equal
    bool equal = true;
    int count = 0;
    for (int i = 0; i < nnz; i++) {
        if (y_vals[i] - y0_vals[i] > 0.01 ) {
            equal = false;
            cout << "Error: " << i << ": " << y_vals[i] << " != " << y0_vals[i] <<  endl;
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