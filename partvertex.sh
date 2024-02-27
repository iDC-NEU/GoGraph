root_path=$(pwd)
files=("indochina")
# files=("$1")
type=reorder

cd ${root_path}/code
g++ PartVexter.cpp -std=c++11 -o PartVexter

for filename in ${files[@]}
do
  # echo -e "\n#PartVexter" 
  efile="${root_path}/dataset/${filename}.mtx.m"
  cmd="${root_path}/code/PartVexter ${efile}.mid.e.gographcluster_v ${efile}.v.map"
  # echo $cmd
  eval $cmd
  echo '------------------------------------------------------------------------'
done