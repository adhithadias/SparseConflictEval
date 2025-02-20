# Steps to run the execution

```bash
git clone 
```


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




# i want to type bash code
```bash
git clone --recurse-submodules -j2 


```