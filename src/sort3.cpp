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

    auto tic = std::chrono::high_resolution_clock::now();
    vector<tuple<int,int,int,double>> vec;

    for (int32_t p = iA; p < pA1_end; p++) {
        int32_t i = A1_crd[p];
        int32_t j = A2_crd[p];
        int32_t k = A3_crd[p];
        double A_ijk = A_vals[p];
        vec.push_back(make_tuple(i,j,k,A_ijk));
    }

    auto tic0 = std::chrono::high_resolution_clock::now();
    sort(vec.begin(), vec.end(), [](const tuple<int,int,int,double> &a, const tuple<int,int,int,double> &b) {
        if (std::get<0>(a) != std::get<0>(b)) return std::get<0>(a) < std::get<0>(b);  // d0
        if (std::get<2>(a) != std::get<2>(b)) return std::get<2>(a) < std::get<2>(b);  // d2
        return std::get<1>(a) < std::get<1>(b);  // d1
    });
    auto tac0 = std::chrono::high_resolution_clock::now();
    auto dur0 = std::chrono::duration_cast<std::chrono::microseconds>(tac0-tic0).count()/1000.0;

    for (long iA = 0; iA < vec.size(); iA++) {
        A1_crd[iA] = std::get<0>(vec[iA]);
        A2_crd[iA] = std::get<1>(vec[iA]);
        A3_crd[iA] = std::get<2>(vec[iA]);
        A_vals[iA] = std::get<3>(vec[iA]);
    }

    auto tac = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(tac-tic).count()/1000.0;
    cout << "Sort time (ms), std sort only (ms)\n";
    cout << dur << ", " << dur0 << endl;


    return 0;
}