root_path=$(pwd)
# files=("$1")
files=("indochina")

cd ${root_path}/code
g++ ProcessIsolatedPoint.cpp -std=c++11 -o ProcessIsolatedPoint

for filename in ${files[@]}
do
  echo -e "\n#ProcessIsolatedPoint_new" 
  efile_1="${root_path}/dataset/${filename}.mtx.m"
  cmd="${root_path}/code/ProcessIsolatedPoint ${efile_1} ${efile_1}.iso ${efile_1}.othervertex_v"
  eval $cmd
  echo '------------------------------------------------------------------------'
done