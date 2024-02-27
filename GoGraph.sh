root_path=$(pwd)
dataset_path=${root_path}/dataset
project_path=$(pwd)

# files=("$1") 
files=("indochina")

# cd ${project_path}

for filename in ${files[@]}
do
    dataset="${dataset_path}/${filename}.mtx.m.mid.e"
    # clustering
    cd ${project_path}/relative_works/rabbit/demo/
    g++ -fopenmp reorder.cc -lnuma -o gographreorder && ./gographreorder ${dataset} 4 -o ${dataset}.cluster

    # organize the clustering results
    cd ${project_path}/relative_works/ 
    g++ GenSuperNodes.cpp -o getSpNode && ./getSpNode ${dataset}.cluster ${dataset_path}/${filename}.spnodes 1

    # generate GoGraph's reordered vertex file: ${dataset}.gograph.v
    cd ${project_path}/code/
    g++ GoGraphCluster.cpp && ./a.out ${dataset} ${dataset_path}/${filename}.spnodes.c_1
done