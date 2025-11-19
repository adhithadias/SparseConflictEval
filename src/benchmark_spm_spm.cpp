#include <random>
#include <chrono>
#include <thread>

#include "mkl_spblas.h"
#include "mkl.h"
#include "taco.h"
#include "input_parser.h"
#include "iterator.h"

#define NEW_PATH false

using namespace taco;
using namespace std;

void print_csr_matrix(sparse_matrix_t A) {
    MKL_INT rows, cols, nnz;
    double *values;
    MKL_INT *row_ptr, *row_end, *col_ind;
    sparse_index_base_t indexing;

    // Get matrix dimensions and number of non-zero elements
    mkl_sparse_d_export_csr(A, &indexing, &rows, &cols, &row_ptr, &row_end, &col_ind, &values);

    std::cout << "CSR Matrix:" << std::endl;
    std::cout << "Rows: " << rows << ", Cols: " << cols << ", NNZ: " << row_ptr[rows] << std::endl;
    std::cout << "Indexing base: " << (indexing == SPARSE_INDEX_BASE_ZERO ? "0" : "1") << std::endl;

    for (MKL_INT i = 0; i < rows; ++i) {
        std::cout << "Row " << i << ": ";
        for (MKL_INT j = row_ptr[i]; j < row_ptr[i + 1]; ++j) {
            std::cout << "(" << col_ind[j] << ", " << values[j] << ") ";
        }
        std::cout << std::endl;
        if (i == 2) {
            break;
        }
    }
}

int main(int argc, char* argv[]) {

    // add code to get the only input matrix from the terminal with -i flag
    
    InputParser input(argc, argv);

    mkl_set_num_threads(1);

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
    std::default_random_engine gen(1000);
    std::uniform_real_distribution<double> unif(0.0, 1.0);

    Format csr({Dense,Sparse});

    Tensor<double> A({n, n}, csr);
    A.setName("A");

    Tensor<double> B({n, n}, csr);
    B.setName("B");

    Tensor<double> C({n, n}, csr);
    C.setName("C");

    // fill B and C with random values based on density
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (unif(gen) < density) {
                B.insert({i, j}, unif(gen));
            }
            if (unif(gen) < density) {
                C.insert({i, j}, unif(gen));
            }
        }
    }

    B.pack();
    C.pack();

    // print B matrix
    

    IndexVar i("i"), j("j"), k("k");
    A(i,j) = B(i,k) * C(k,j);

    IndexStmt stmt = A.getAssignment().concretize(NEW_PATH);
    A.setNewPath(NEW_PATH);
    A.setAssembleWhileCompute(false);
    A.compile(stmt, false);
    A.assemble();
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

    // print A
    


    // annonymous function to find median
    auto find_median = [](std::vector<std::chrono::microseconds> &v) {
        std::sort(v.begin(), v.end(), std::less<std::chrono::microseconds>());
        return v[v.size()/2].count() / 1000.0;
    };

    // // find median from time_to_compute_fuse
    double fused_median = find_median(times);

    
    cout << "Fused median: " << fused_median << " ms" << endl;

    taco_tensor_t* b_storage = B.getStorage();
    taco_tensor_t* c_storage = C.getStorage();


    MKL_INT m0 = (MKL_INT) B.getDimension(0);
    MKL_INT k0 = (MKL_INT) B.getDimension(1);
    MKL_INT n0 = (MKL_INT) C.getDimension(1);
    std::cout << "Matrix B: " << m0 << " x " << k0 << std::endl;
    std::cout << "Matrix C: " << k0 << " x " << n0 << std::endl;


    int* b_rows = (int*) b_storage->indices[1][0];
    std::cout << "1\n";
    // malloc MKL_INT* B_rows with size m0
    MKL_INT* B_rows = (MKL_INT*) malloc((n+1) * sizeof(MKL_INT));
    for (int i = 0; i <= n; i++) {
        B_rows[i] = (MKL_INT) b_rows[i];
        // std::cout << B_rows[i] << ", ";
    }
    std::cout << std::endl;
    int nnz = b_rows[n];
    std::cout << "B nnz: " << nnz << std::endl;
    int* b_cols = (int*) b_storage->indices[1][1];
    MKL_INT* B_cols = (MKL_INT*) malloc(nnz * sizeof(MKL_INT));
    for (int i = 0; i < nnz; i++) {
        B_cols[i] = (MKL_INT) b_cols[i];
        // std::cout << B_cols[i] << ", ";
    }
    std::cout << std::endl;
    double* B_vals = (double*) b_storage->vals;

    int* c_rows = (int*) c_storage->indices[1][0];
    MKL_INT* C_rows = (MKL_INT*) malloc((n+1) * sizeof(MKL_INT));
    for (int i = 0; i <= n; i++) {
        C_rows[i] = (MKL_INT) c_rows[i];
        // std::cout << C_rows[i] << ", ";
    }
    std::cout << std::endl;
    nnz = c_rows[n];
    int* c_cols = (int*) c_storage->indices[1][1];
    MKL_INT* C_cols = (MKL_INT*) malloc(nnz * sizeof(MKL_INT));
    for (int i = 0; i < nnz; i++) {
        C_cols[i] = (MKL_INT) c_cols[i];
        // std::cout << C_cols[i] << ", ";
    }
    std::cout << std::endl;
    double* C_vals = (double*) c_storage->vals;

    // return 0;

    sparse_matrix_t B_mkl, C_mkl, A_mkl;
    struct matrix_descr descrA, descrB, descrC;
    descrA.type = SPARSE_MATRIX_TYPE_GENERAL;
    descrB.type = SPARSE_MATRIX_TYPE_GENERAL;
    descrC.type = SPARSE_MATRIX_TYPE_GENERAL;
    
    sparse_status_t status;
    status = mkl_sparse_d_create_csr(&B_mkl, SPARSE_INDEX_BASE_ZERO, m0, k0,
                                     B_rows, B_rows + 1, B_cols, B_vals);
    if (status != SPARSE_STATUS_SUCCESS) {
        std::cout << "Error creating B_mkl matrix: " << status << std::endl;
        return 1;
    }
    // print_csr_matrix(B_mkl);
    
    status = mkl_sparse_d_create_csr(&C_mkl, SPARSE_INDEX_BASE_ZERO, k0, n0,
                                     C_rows, C_rows + 1, C_cols, C_vals);
    if (status != SPARSE_STATUS_SUCCESS) {
        std::cout << "Error creating C_mkl matrix: " << status << std::endl;
        mkl_sparse_destroy(B_mkl);
        return 1;
    }

    std::cout << "MKL SpMM: " << m0 << " x " << n0 << " = "
              << m0 << " x " << k0 << " * " << k0 << " x " << n0 << std::endl;

    std::vector<std::chrono::microseconds> times_mkl;
    for (int r = 0; r < repetitions-1; r++) {
        auto start = std::chrono::high_resolution_clock::now();
        status = mkl_sparse_spmm(SPARSE_OPERATION_NON_TRANSPOSE, B_mkl, C_mkl, &A_mkl);
        mkl_sparse_order(A_mkl);
        auto end = std::chrono::high_resolution_clock::now();
        
        if (status != SPARSE_STATUS_SUCCESS) {
            std::cout << "Error in mkl_sparse_spmm: " << status << std::endl;
            break;
        }
        
        times_mkl.push_back(
            std::chrono::duration_cast<std::chrono::microseconds>(end-start));
        std::cout << "MKL Time: " << times_mkl[r].count() / 1000.0 << " ms" << std::endl;
        mkl_sparse_destroy(A_mkl);
    }

    // print_csr_matrix(B_mkl);
    // print_csr_matrix(C_mkl);
    // print_csr_matrix(A_mkl);

    double mkl_median = find_median(times_mkl);
    cout << "MKL median: " << mkl_median << " ms" << endl;
    
    // Clean up MKL matrices
    mkl_sparse_destroy(B_mkl);
    mkl_sparse_destroy(C_mkl);
    
    return 0;
}