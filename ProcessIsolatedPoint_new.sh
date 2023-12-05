# ##############################################################################
root_path=/home/liupengxi/code/GoGraph
files=("$1") # web-indochina-2004 sk-2005 google cit-Patents wikilink
type=reorder

cd $root_path
g++ ProcessIsolatedPoint_new.cpp -std=c++11 -o ProcessIsolatedPoint_new

for filename in ${files[@]}
do
  echo -e "\n#ProcessIsolatedPoint_new" 
  # efile="/home/liupengxi/dataset/large/${filename}/${filename}.mtx.${type}.e"
  # efile="/home/liupengxi/dataset/large/${filename}/${filename}.mtx.origin_dis.e"
 # efile="/home/liupengxi/code/GoGraph/DateSet/test_3.e"
   efile_1="/home/liupengxi/dataset/large/${filename}/${filename}.mtx"
   #efile_2="/home/liupengxi/dataset/large/${filename}/${filename}.mtx.${type}.e.processvexter"
   cmd="${root_path}/ProcessIsolatedPoint_new ${efile_1}.m ${efile_1}.iso ${efile_1}.m.othervexters_v"
  # cmd="${root_path}/ProcessIsolatedPoint_new ${efile_1}.m ${efile_1}.isolated ${efile_1}.m.othervexters_v"
#  cmd="${root_path}/ProcessIsolatedPoint_new ${efile} ${efile}.isolated.v ${efile}.order.v"
  echo $cmd
  eval $cmd
  echo '------------------------------------------------------------------------'
done