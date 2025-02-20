#include "iterator.h"


int iterateSSS(taco_tensor_t *A) {
    int* __restrict A1_pos = (int*)(A->indices[0][0]);
    int* __restrict A1_crd = (int*)(A->indices[0][1]);
    int* __restrict A2_pos = (int*)(A->indices[1][0]);
    int* __restrict A2_crd = (int*)(A->indices[1][1]);
    int* __restrict A3_pos = (int*)(A->indices[2][0]);
    int* __restrict A3_crd = (int*)(A->indices[2][1]);
    double* __restrict A_vals = (double*)(A->vals);

    int nnz = 0;

    for (int32_t iA = A1_pos[0]; iA < A1_pos[1]; iA++) {
        int32_t i = A1_crd[iA];
        for (int32_t jA = A2_pos[iA]; jA < A2_pos[(iA + 1)]; jA++) {
        int32_t j = A2_crd[jA];
            for (int32_t kA = A3_pos[jA]; kA < A3_pos[(jA + 1)]; kA++) {
                int32_t k = A3_crd[kA];
                nnz++;
                cout << "A[" << i << "," << j << "," << k << "] = " << A_vals[kA] << endl;
            }
        }
    }

    return nnz;
}

int iterate_csr(taco_tensor_t *A, bool print) {
    int A1_dimension = (int)(A->dimensions[0]);
    int* __restrict A2_pos = (int*)(A->indices[1][0]);
    int* __restrict A2_crd = (int*)(A->indices[1][1]);
    double* __restrict A_vals = (double*)(A->vals);

    for (int32_t i = 0; i < A1_dimension; i++) {
        for (int32_t jA = A2_pos[i]; jA < A2_pos[i+1]; jA++) {
            int32_t j = A2_crd[jA];
            double A_ij = A_vals[jA];
            if (print)
            std::cout << "A[" << i << "," << j << "] = " << A_ij << std::endl;
        }
    }

    return 0;
}

int check_equality_sss(taco_tensor_t *A, taco_tensor_t *B) {
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

    int nnz = 0;

    assert (A->dimensions[0] == B->dimensions[0]);
    assert (A->dimensions[1] == B->dimensions[1]);
    assert (A->dimensions[2] == B->dimensions[2]);


    if (A1_pos[1] != B1_pos[1]) {
        cout << "Error: " << A1_pos[1] << " != " << B1_pos[1] << endl;
    }
    for (int32_t iA = A1_pos[0]; iA < A1_pos[1]; iA++) {
        int32_t i = A1_crd[iA];
        int32_t iB = B1_crd[iA];

        if (i != iB || A2_pos[iA] != B2_pos[iA] || A2_pos[iA + 1] != B2_pos[iA + 1]) {
            cout << "Error: " << i << " != " << iB << " || " << A2_pos[iA] << " != " << B2_pos[iA] << " || " << A2_pos[iA + 1] << " != " << B2_pos[iA + 1] << endl;
            break;
        }
        for (int32_t jA = A2_pos[iA]; jA < A2_pos[(iA + 1)]; jA++) {
            int32_t j = A2_crd[jA];
            int32_t jB = B2_crd[jA];
            if (j != jB || A3_pos[jA] != B3_pos[jA] || A3_pos[jA + 1] != B3_pos[jA + 1]) {
                cout << "Error: " << j << " != " << jB << " || " << A3_pos[jA] << " != " << B3_pos[jA] << " || " << A3_pos[jA + 1] << " != " << B3_pos[jA + 1] << endl;
                break;
            }
            for (int32_t kA = A3_pos[jA]; kA < A3_pos[(jA + 1)]; kA++) {
                int32_t k = A3_crd[kA];
                int32_t kB = B3_crd[kA];
                if (k != kB || A_vals[kA] != B_vals[kA]) {
                    cout << "Error: " << k << " != " << kB << " || " << A_vals[kA] << " != " << B_vals[kA] << endl;
                    break;
                }
                nnz++;
            }
        }
    }
    cout << "Output tensors are equal nnz: " << nnz << endl;

    return nnz;
}

int iterateCSS(taco_tensor_t *A) {
  int* __restrict A1_pos = (int*)(A->indices[0][0]);
  int* __restrict A1_crd = (int*)(A->indices[0][1]);
  int* __restrict A2_crd = (int*)(A->indices[1][1]);
  int* __restrict A3_crd = (int*)(A->indices[2][1]);
  double* __restrict A_vals = (double*)(A->vals);

    int32_t iA = A1_pos[0];
  int32_t pA1_end = A1_pos[1];

  for (int32_t p = iA; p < pA1_end; p++) {
    int32_t i = A1_crd[p];
    int32_t j = A2_crd[p];
    int32_t k = A3_crd[p];
    double A_ijk = A_vals[p];
    cout << "A[" << i << "," << j << "," << k << "] = " << A_ijk << endl;
  }
  return 0;
}

int iterateDSS(taco_tensor_t *A) {
    int A1_dimension = (int)(A->dimensions[0]);
    int* __restrict A2_pos = (int*)(A->indices[1][0]);
    int* __restrict A2_crd = (int*)(A->indices[1][1]);
    int* __restrict A3_pos = (int*)(A->indices[2][0]);
    int* __restrict A3_crd = (int*)(A->indices[2][1]);
    double* __restrict A_vals = (double*)(A->vals);

    for (int i = 0; i < A1_dimension; i++) {
        for (int jA = A2_pos[i]; jA < A2_pos[i+1]; jA++) {
            int j = A2_crd[jA];
            for (int kA = A3_pos[j]; kA < A3_pos[j+1]; kA++) {
                int k = A3_crd[kA];
                double A_ijk = A_vals[kA];
                cout << "A[" << i << "," << j << "," << k << "] = " << A_ijk << endl;
            }
        }
    }

    return 0;
}

void check_equality(int nnz, double* y_vals, double* y_copy_vals) {
    int count = 0;
    for (int i = 0; i < nnz; i++) {
        if (y_vals[i] != y_copy_vals[i]) {
            cout << "Error: " << i << ": " << y_vals[i] << " != " << y_copy_vals[i] <<  endl;
            break;
        } else {
            count++;
        }
    }
    cout << "Output tensors are equal count: " << count << endl;
}