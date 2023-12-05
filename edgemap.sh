#!/bin/bash
dataset_path=/home/liupengxi
project_path=/home/liupengxi/code/GoGraph
files=("$1") #web-Google web-indochina-2004 web-sk-2005 #web-indochina-2004 sk-2005 google cit-Patents wikilink
shift
algs=("$@") #DegSort HubSort HubCluster Gorder Rabbit PartGoGraph
cd ${project_path}

for filename in ${files[@]}
do
    # dataset="${dataset_path}/dataset/large/${filename}/${filename}.mtx.${type}.e"
    # dataset="${dataset_path}/dataset/large/${filename}/${filename}.mtx.${type}.e.processvexter.e.${type}.e"

    dataset="${dataset_path}/dataset/large/${filename}/${filename}.mtx.m" 
    input_edge=${dataset}
    g++ edgemap.cpp -o edgemap
    for algname in ${algs[@]}
    do
      input_vexter="${dataset}.${algname}_v"
      echo -e "--- start\n"
      ./edgemap ${input_vexter} ${input_edge}
      echo -e "--- finished\n\n"
    done
    # dataset="${dataset_path}/dataset/large/${filename}/${filename}.mtx.reorder.e"
    # input_vexter="${dataset}.PartGoGraph_v"
    # input_edge=${dataset}

    # echo -e "--- start\n"
    # g++ edgemap.cpp -o edgemap && ./edgemap ${input_vexter} ${input_edge}
    # echo -e "--- finished\n\n"

done
