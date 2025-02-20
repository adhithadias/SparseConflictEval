#include <algorithm>
#include "taco.h"

#include "input_parser.h"
#include "iterator.h"

using namespace taco;

static Type matrixType(Float64, {3,3});



int main(int argc, char* argv[]) {

    InputParser input(argc, argv);
    if (!input.cmdOptionExists("-f")) {
        std::cout << "No input matrix specified. Specify with -f <filename> Exiting..." << std::endl;
        return 1;
    }
    const std::string &filename = input.getCmdOption("-f");

    ModeFormat compressedNU = ModeFormat::Compressed(ModeFormat::NOT_UNIQUE);
    ModeFormat singletonNU = ModeFormat::Singleton(ModeFormat::NOT_UNIQUE);

    Format coo({compressedNU, singletonNU});
    Format csr({Dense, Sparse});
    Tensor<double> A = read(filename, coo);

    taco_tensor_t* a_storage = A.getStorage();
    // int n = iterate_csr(A.getStorage(), false);

    int a1_dimension = (int)(a_storage->dimensions[0]);
    int* __restrict A1_pos = (int*)(a_storage->indices[0][0]);
    int* __restrict A1_crd = (int*)(a_storage->indices[0][1]);
    int* __restrict A2_crd = (int*)(a_storage->indices[1][1]);
    double* __restrict A_vals = (double*)(a_storage->vals);

    int32_t iA = A1_pos[0];
    int32_t pA1_end = A1_pos[1];

    auto tic = std::chrono::high_resolution_clock::now();
    // vector<tuple<int,int,double>> vec;


    struct points {
        int l;
        double val;
    };

    points* vec = new points[pA1_end - iA];

    for (int32_t p = iA; p < pA1_end; p++) {
        vec[p-iA].l = A2_crd[p] * a1_dimension + A1_crd[p];
        vec[p-iA].val = A_vals[p];
        // int32_t i = A1_crd[p];
        // int32_t j = A2_crd[p];
        // double A_ijk = A_vals[p];
        // vec.push_back(make_tuple(i,j,A_ijk));
    }

    auto tic0 = std::chrono::high_resolution_clock::now();
    // sort(vec, pA1_end-, [](const points &a, const points &b) {
    //     if (a.i != b.i) return a.i < b.i;  // d0
    //     return a.j < b.j;  // d1
    // });
    // sort(vec.begin(), vec.end(), [](const tuple<int,int,double> &a, const tuple<int,int,double> &b) {
    //     if (std::get<1>(a) != std::get<1>(b)) return std::get<1>(a) < std::get<1>(b);  // d1
    //     return std::get<0>(a) < std::get<0>(b);  // d0
    // });


    qsort(vec, pA1_end - iA, sizeof(points), [](const void *a, const void *b) {
        points* pa = (points*)a;
        points* pb = (points*)b;
        return pa->l - pb->l;
    });
    auto tac0 = std::chrono::high_resolution_clock::now();
    auto dur0 = std::chrono::duration_cast<std::chrono::microseconds>(tac0-tic0).count()/1000.0;

    for (long iA = 0; iA < pA1_end; iA++) {
        A1_crd[iA] = vec[iA].l % a1_dimension;
        A2_crd[iA] = vec[iA].l / a1_dimension;
        A_vals[iA] = vec[iA].val;

        // A1_crd[iA] = std::get<0>(vec[iA]);
        // A2_crd[iA] = std::get<1>(vec[iA]);
        // A_vals[iA] = std::get<2>(vec[iA]);
    }

    auto tac = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(tac-tic).count()/1000.0;
    cout << "Time to sort: " << dur << " ms, sort only: " << dur0 << endl;

    // cout << "Sorted A" << endl;
    // for (auto &t : vec) {
    //     cout << "A[" << std::get<0>(t) << "," << std::get<2>(t) << "," << std::get<1>(t) << "] = " << std::get<3>(t) << endl;
    // }


    return 0;
}