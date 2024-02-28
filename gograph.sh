root_path=$(pwd)
dataset_path=${root_path}/dataset
project_path=$(pwd)

files=("$1") 

for filename in ${files[@]}
do
    echo -e "#gograph" 
    dataset="${dataset_path}/${filename}.m.mid.e"
    # clustering
    cd ${project_path}/relative_works/rabbit/demo/
    g++ -fopenmp reorder.cc -lnuma -o gographreorder.out && ./gographreorder.out ${dataset} 4 -o ${dataset}.cluster

    # organize the clustering results
    cd ${project_path}/relative_works/ 
    g++ GenSuperNodes.cpp -o getSpNode.out && ./getSpNode.out ${dataset}.cluster ${dataset_path}/${filename}.spnodes 1

    # generate GoGraph's reordered vertex file: ${dataset}.gograph.v
    cd ${project_path}/code/
    g++ GoGraphCluster.cpp -o GoGraphCluster.out && ./GoGraphCluster.out ${dataset} ${dataset_path}/${filename}.spnodes.c_1
done