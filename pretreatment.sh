root_path=$(pwd)

files=("$1")

cd ${root_path}/code
g++ processdata.cpp -std=c++11 -o processdata.out

g++ pretreatment.cpp -std=c++11 -o pretreatment.out

for filename in ${files[@]}
do
  echo -e "#pretreatment" 
  efile="${root_path}/dataset/${filename}"
  eval "${root_path}/code/processdata.out ${efile}"
  eval "${root_path}/code/pretreatment.out ${efile}.m"
  echo '------------------------------------------------------------------------'
done