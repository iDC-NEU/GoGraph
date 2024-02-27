root_path=$(pwd)
# files=("$1")
files=("indochina")

cd ${root_path}/code
g++ ProcessBigdegreePoint.cpp -std=c++11 -o ProcessBigdegreePoint

for filename in ${files[@]}
do
  echo -e "\n#ProcessBigdegreePoint" 
  efile_1="${root_path}/dataset/${filename}.mtx.m"
  cmd="${root_path}/code/ProcessBigdegreePoint ${efile_1} ${efile_1}.big ${efile_1}.mid.e.gographcluster_v.mid.v"
  eval $cmd
  echo '------------------------------------------------------------------------'
done