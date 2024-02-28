root_path=$(pwd)
files=("$1")
type=reorder

cd ${root_path}/code
g++ PartVexter.cpp -std=c++11 -o PartVexter.out

for filename in ${files[@]}
do
  echo -e "\n#partvertex" 
  efile="${root_path}/dataset/${filename}.m"
  cmd="${root_path}/code/PartVexter.out ${efile}.mid.e.gographcluster_v ${efile}.v.map"
  # echo $cmd
  eval $cmd
  echo '------------------------------------------------------------------------'
done