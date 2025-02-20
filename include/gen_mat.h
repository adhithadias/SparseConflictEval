#include <string>
#include <random>

#include "taco.h"

using namespace std;

void gen_3d_tns(int m, int n, int o, float sparsity, std::string& filename);

void gen_3d_tns(int m, int n, int o, float sparsity, std::string& filename) {
    std::default_random_engine gen(0);
    std::uniform_real_distribution<double> unif(0.0, 1.0);

    Format csf({Sparse, Sparse, Sparse});
    Tensor<double> A({m, n, o}, csf);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < o; k++) {
                float rand_float = (float)rand()/(float)(RAND_MAX);
                if (rand_float > sparsity) 
                    A.insert({i, j, k}, unif(gen));
            }
        }
    }
    A.pack();

    write(filename, A);

}