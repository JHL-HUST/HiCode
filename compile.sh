#! /usr/bin/env bash

# Compile Base Algorithms
cd ./algs/Infomap
make
cd ../..

g++ -o ./algs/link_clustering/dataTrans ./algs/link_clustering/dataTrans.cpp
g++ -o ./algs/link_clustering/calcAndWrite_Jaccards ./algs/link_clustering/calcAndWrite_Jaccards.cpp 
g++ -o ./algs/link_clustering/callCluster ./algs/link_clustering/callCluster.cpp 

cd ./algs/OSLOM2
sh ./compile_all.sh
cd ../..

cd ./algs/Modularity
make all
cd ../..

# Compile HICODE
make Main