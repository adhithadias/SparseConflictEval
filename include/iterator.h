#include "taco.h"

using namespace taco;
using namespace std;

int iterateSSS(taco_tensor_t *A);
int iterateCSS(taco_tensor_t *A);
int iterateDSS(taco_tensor_t *A);
void check_equality(int nnz, double* y_vals, double* y_copy_vals);
int check_equality_sss(taco_tensor_t *A, taco_tensor_t *B);
int iterate_csr(taco_tensor_t *A, bool print = true);