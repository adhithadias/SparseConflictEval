# Dependencies

1. SparseConflict
2. CMake
3. Git

# Building the project

```bash
# build SparseConflict
export TACO_ROOT=</path/to/SparseConflict/project/directory>
bash build_taco.sh
bash build_project.sh
```

# Kernels

This repository evaluates the following kernels against TACO.

|---------------------------------------------------------------|
|  Kernel                           | Name/File                 |  
|---------------------------------------------------------------|
| y(i) = A(i,j) * B(j,i)            | dotprod-denseout          |
| y: Dense                          |                           |
| A, B: CSR                         |                           |
|---------------------------------------------------------------|
| Y(i,j) = A(i,j) * B(j,i)          | elementwise-mul           |
| Y, A, B: CSR                      |                           |
|---------------------------------------------------------------|
| y(i) = A(i,j,k) * B(i,k,j)        | tensorcontract-1dout      |
| y: Dense                          |                           |
| A, B: CSF                         |                           |
|---------------------------------------------------------------|
| Y(i,j,k) = A(i,j,k) * B(i,k,j)    | 3d-elwisemul              |
| Y, A, B: CSF                      |                           |
|---------------------------------------------------------------|
