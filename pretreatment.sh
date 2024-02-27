root_path=$(pwd)

# files=("$1")
files=("indochina")

cd ${root_path}/code
g++ processdata.cpp -std=c++11 -o processdata

cd ${root_path}/code
g++ pretreatment.cpp -std=c++11 -o pretreatment

for filename in ${files[@]}
do
  echo -e "#pretreatment" 
  efile="${root_path}/dataset/${filename}.mtx"
  eval "${root_path}/code/processdata ${efile}"
  eval "${root_path}/code/pretreatment ${efile}.m"
  echo '------------------------------------------------------------------------'
done