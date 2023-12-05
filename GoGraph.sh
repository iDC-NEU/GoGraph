#!/bin/bash
dataset_path=/home/liupengxi
project_path=/home/liupengxi/code/GoGraph
files=("$1") #web-indochina-2004 sk-2005 google cit-Patents wikilink
# dataset="${dataset_path}/dataset/large/${filename}/${filename}.mtx.origin_dis.e"
type=reorder
cd ${project_path}

# 整理数据集，如果数据集已整理好则不需要这步
# g++ ./processdata.cpp && ./a.out ${dataset}
# echo -e "--- process dataset finished\n\n"

#input_edge=${dataset}.mod
#input_edge=${dataset}

for filename in ${files[@]}
do
    # dataset="${dataset_path}/dataset/large/${filename}/${filename}.mtx.${type}.e"
    # dataset="${dataset_path}/dataset/large/${filename}/${filename}.mtx.${type}.e.processvexter.e.${type}.e"
    dataset="${dataset_path}/dataset/large/${filename}/${filename}.mtx.mid.e"
    input_edge=${dataset}
    # 1.聚类 (rabbit的部分代码被修改，需要进行替换)
    cd ${project_path}/relative_works/rabbit_order-master/demo/
    ./gographreorder ${input_edge} 4 -o ${input_edge}.cluster
    echo -e "--- incremental aggregation to find cluster finished\n\n"

    # 2.整理聚类结果
    cd ${project_path}/relative_works/ 
    g++ GenSuperNodes.cpp -o getSpNode && ./getSpNode ${input_edge}.cluster ${input_edge}.spnodes 1
    echo -e "--- get super nodes finished\n\n"

    # 3.生成GoGraph新代码的排序顶点文件: 
    #     输出文件为: ${input_edge}.gograph.v
    cd ${project_path}
    g++ GoGraphCluster.cpp && ./a.out ${input_edge} ${input_edge}.spnodes.c_1
    echo -e "--- use GoGraphCluster to reorder finished\n\n"
done
# 运行测试pagerank测试排序效果
# g++ pagerank.cpp  && ./a.out ${input_edge} ${input_edge}.gograph.v 
# echo -e "--- test pagerank finished\n\n"