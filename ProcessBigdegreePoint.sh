root_path=$(pwd)
files=("$1")

cd ${root_path}/code
g++ ProcessBigdegreePoint.cpp -std=c++11 -o ProcessBigdegreePoint.out

for filename in ${files[@]}
do
  echo -e "\n#ProcessBigdegreePoint" 
  efile_1="${root_path}/dataset/${filename}.m"
  cmd="${root_path}/code/ProcessBigdegreePoint.out ${efile_1} ${efile_1}.big ${efile_1}.mid.e.gographcluster_v.mid.v"
  eval $cmd
  echo '------------------------------------------------------------------------'
done