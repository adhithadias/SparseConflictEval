#include "taco.h"

#include "input_parser.h"
#include "iterator.h"

using namespace taco;

static Type matrixType(Float64, {3,3});

int main(int argc, char* argv[]) {

    InputParser input(argc, argv);
    if (!input.cmdOptionExists("-f")) {
        std::cout << "No input matrix specified. Specify with -i <filename> Exiting..." << std::endl;
        return 1;
    }
    const std::string &filename = input.getCmdOption("-f");

    ModeFormat compressedNU = ModeFormat::Compressed(ModeFormat::NOT_UNIQUE);
    ModeFormat singletonNU = ModeFormat::Singleton(ModeFormat::NOT_UNIQUE);

    Format coo({compressedNU, singletonNU, singletonNU});
    Format csf({Sparse, Sparse, Sparse});
    Tensor<double> A = read(filename, coo);

    taco_tensor_t* a_storage = A.getStorage();
    // iterateCSS(A.getStorage());

    int* __restrict A1_pos = (int*)(a_storage->indices[0][0]);
    int* __restrict A1_crd = (int*)(a_storage->indices[0][1]);
    int* __restrict A2_crd = (int*)(a_storage->indices[1][1]);
    int* __restrict A3_crd = (int*)(a_storage->indices[2][1]);
    double* __restrict A_vals = (double*)(a_storage->vals);

    int32_t iA = A1_pos[0];
    int32_t pA1_end = A1_pos[1];

    auto dim1 = A.getDimensions()[0];
    auto dim2 = A.getDimensions()[1];
    auto dim3 = A.getDimensions()[2];
    auto nnz = pA1_end-iA;
    cout << "dimensions: " << A.getDimensions()[0] << ", " << A.getDimensions()[1] << ", " << A.getDimensions()[2] << endl;
    cout << "number of nonzeros: " << a_storage->vals_size << endl;
    cout << "number of nonzeros: " << pA1_end-iA << endl;

    struct coord {
        int i;
        int j;
        int k;
        double val;
    };

    auto tic = std::chrono::high_resolution_clock::now();
    auto* aos = (coord*)malloc((pA1_end-iA)*sizeof(coord));

    for (int32_t p = iA; p < pA1_end; p++) {
        int32_t i = A1_crd[p];
        int32_t j = A2_crd[p];
        int32_t k = A3_crd[p];
        double A_ijk = A_vals[p];
        aos[p-iA] = {i,j,k,A_ijk};
    }

    auto comparator = [](const void* a, const void* b) {
        coord* aa = (coord*)a;
        coord* bb = (coord*)b;
        if (aa->i != bb->i) return aa->i - bb->i;
        if (aa->k != bb->k) return aa->k - bb->k;
        return aa->j - bb->j;
    };

    auto tic0 = std::chrono::high_resolution_clock::now();
    qsort(aos, pA1_end-iA, sizeof(coord), comparator);
    auto tac0 = std::chrono::high_resolution_clock::now();
    auto dur0 = std::chrono::duration_cast<std::chrono::microseconds>(tac0-tic0).count()/1000.0;

    for (int32_t p = iA; p < pA1_end; p++) {
        A1_crd[p] = aos[p-iA].i;
        A2_crd[p] = aos[p-iA].j;
        A3_crd[p] = aos[p-iA].k;
        A_vals[p] = aos[p-iA].val;
    }

    auto tac = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(tac-tic).count()/1000.0;
    cout << "m, n, k, nnz, sparsity\n";
    cout << dim1 
        << ", " << dim2 
        << ", " << dim3 
        << ", " << nnz 
        << ", " << std::scientific << (double)nnz / ((double)dim1 * dim2 * dim3) 
        << endl;
    cout << "Sort time (ms), std sort only (ms)\n";
    cout << dur << ", " << dur0 << endl;

    return 0;
}