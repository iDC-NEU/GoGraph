#!/bin/bash
root_path=$(pwd)
dataset_path=${root_path}/dataset
project_path=${root_path}/code
# files=("$1")
files=("indochina")
shift
# algs=("$@") #DegSort HubSort HubCluster Gorder Rabbit PartGoGraph
algs=("GoGraph") #DegSort HubSort HubCluster Gorder Rabbit PartGoGraph
cd ${project_path}

for filename in ${files[@]}
do

    dataset="${dataset_path}/${filename}.mtx.m" 
    input_edge=${dataset}
    g++ edgemap.cpp -o edgemap
    for algname in ${algs[@]}
    do
      input_vertex="${dataset}.${algname}_v"
      echo -e "--- start\n"
      ./edgemap ${input_vertex} ${input_edge}
      echo -e "--- finished\n\n"
    done

  rm ${dataset}.b.e
  rm ${dataset}.big
  rm ${dataset}.iso
  rm ${dataset}.mid.e
  rm ${dataset}.mid.e.cluster
  rm ${dataset}.mid.e.gographcluster_v
  rm ${dataset}.mid.e.gographcluster_v.mid.v
  rm ${dataset}.othervertex_v
  rm ${dataset}.GoGraph_v
  rm ${dataset}.v.map
  rm "${dataset_path}/${filename}.spnodes.c_1"

done

 