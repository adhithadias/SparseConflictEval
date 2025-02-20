!#/bin/bash

mkdir -p ../tar-files
mkdir -p ../matrices

wget -nc --directory-prefix=../tar-files https://www.cise.ufl.edu/research/sparse/MM/Boeing/pwtk.tar.gz
tar -xvf ../tar-files/pwtk.tar.gz -C ../tar-files
mv ../tar-files/pwtk/* ../matrices/
rm -rf ../tar-files/pwtk

wget -nc --directory-prefix=../tar-files https://suitesparse-collection-website.herokuapp.com/MM/Williams/webbase-1M.tar.gz
tar -xvf ../tar-files/webbase-1M.tar.gz -C ../tar-files
mv ../tar-files/webbase-1M/* ../matrices/
rm -rf ../tar-files/webbase-1M

wget -nc --directory-prefix=../tar-files https://suitesparse-collection-website.herokuapp.com/MM/QLi/majorbasis.tar.gz
tar -xvf ../tar-files/majorbasis.tar.gz -C ../tar-files
mv ../tar-files/majorbasis/* ../matrices/
rm -rf ../tar-files/majorbasis

wget -nc --directory-prefix=../tar-files https://suitesparse-collection-website.herokuapp.com/MM/Lin/Lin.tar.gz
tar -xvf ../tar-files/Lin.tar.gz -C ../tar-files
mv ../tar-files/Lin/* ../matrices/
rm -rf ../tar-files/Lin

wget -nc --directory-prefix=../tar-files https://suitesparse-collection-website.herokuapp.com/MM/McRae/ecology1.tar.gz
tar -xvf ../tar-files/ecology1.tar.gz -C ../tar-files
mv ../tar-files/ecology1/* ../matrices/
rm -rf ../tar-files/ecology1

wget -nc --directory-prefix=../tar-files https://suitesparse-collection-website.herokuapp.com/MM/Bourchtein/atmosmodd.tar.gz
tar -xvf ../tar-files/atmosmodd.tar.gz -C ../tar-files
mv ../tar-files/atmosmodd/* ../matrices/
rm -rf ../tar-files/atmosmodd


wget -nc --directory-prefix=../tar-files https://suitesparse-collection-website.herokuapp.com/MM/Bourchtein/atmosmodm.tar.gz
tar -xvf ../tar-files/atmosmodm.tar.gz -C ../tar-files
mv ../tar-files/atmosmodm/* ../matrices/
rm -rf ../tar-files/atmosmodm


wget -nc --directory-prefix=../tar-files https://suitesparse-collection-website.herokuapp.com/MM/SNAP/com-Orkut.tar.gz
tar -xvf ../tar-files/com-Orkut.tar.gz -C ../tar-files
mv ../tar-files/com-Orkut/* ../matrices/
rm -rf ../tar-files/com-Orkut


wget -nc --directory-prefix=../tar-files https://suitesparse-collection-website.herokuapp.com/MM/QLi/largebasis.tar.gz
tar -xvf ../tar-files/largebasis.tar.gz -C ../tar-files
mv ../tar-files/largebasis/* ../matrices/
rm -rf ../tar-files/largebasis

wget -nc --directory-prefix=../tar-files https://suitesparse-collection-website.herokuapp.com/MM/VLSI/vas_stokes_1M.tar.gz
tar -xvf ../tar-files/vas_stokes_1M.tar.gz -C ../tar-files
mv ../tar-files/vas_stokes_1M/* ../matrices/
rm -rf ../tar-files/vas_stokes_1M


wget -nc --directory-prefix=../tar-files https://suitesparse-collection-website.herokuapp.com/MM/VLSI/ss.tar.gz
tar -xvf ../tar-files/ss.tar.gz -C ../tar-files
mv ../tar-files/ss/* ../matrices/
rm -rf ../tar-files/ss


wget -nc --directory-prefix=../tar-files https://suitesparse-collection-website.herokuapp.com/MM/JGD_Kocay/Trec13.tar.gz
tar -xvf ../tar-files/Trec13.tar.gz -C ../tar-files
mv ../tar-files/Trec13/* ../matrices/
rm -rf ../tar-files/Trec13