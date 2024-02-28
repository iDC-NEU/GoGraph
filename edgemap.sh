#!/bin/bash
root_path=$(pwd)
dataset_path=${root_path}/dataset
project_path=${root_path}/code
files=("$1")
shift
algs=("GoGraph")
cd ${project_path}

for filename in ${files[@]}
do
  echo -e "#edgemap" 
  dataset_m="${dataset_path}/${filename}.m" 
  input_edge=${dataset_m}
  g++ edgemap.cpp -o edgemap.out
  for algname in ${algs[@]}
  do
    input_vertex="${dataset_m}.${algname}_v"
    echo -e "--- start\n"
    ./edgemap ${input_vertex} ${input_edge}
    echo -e "--- finished\n\n"
  done

  rm ${dataset_m}.b.e
  rm ${dataset_m}.big
  rm ${dataset_m}.iso
  rm ${dataset_m}.mid.e
  rm ${dataset_m}.mid.e.cluster
  rm ${dataset_m}.mid.e.gographcluster_v
  rm ${dataset_m}.mid.e.gographcluster_v.mid.v
  rm ${dataset_m}.othervertex_v
  rm ${dataset_m}.GoGraph_v
  rm ${dataset_m}.v.map
  rm ${dataset_m}
  rm "${dataset_path}/${filename}.spnodes.c_1"

  mv ${dataset_m}.GoGraph_v.reorder.e ${dataset_path}/${filename}.GoGraph

done

 