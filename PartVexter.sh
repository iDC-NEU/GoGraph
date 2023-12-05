# ##############################################################################
root_path=/home/liupengxi/code/GoGraph
files=("$1") # web-indochina-2004 sk-2005 google cit-Patents wikilink
type=reorder

cd $root_path
g++ PartVexter.cpp -std=c++11 -o PartVexter

for filename in ${files[@]}
do
  echo -e "\n#PartVexter" 
  # efile="/home/liupengxi/dataset/large/${filename}/${filename}.mtx.${type}.e"
  # efile="/home/liupengxi/dataset/large/${filename}/${filename}.mtx.origin_dis.e"
  # efile="/home/liupengxi/code/GoGraph/DateSet/test.e"
  efile="/home/liupengxi/dataset/large/${filename}/${filename}.mtx"
  cmd="${root_path}/PartVexter ${efile}.mid.e.gographcluster_v ${efile}.v.map"
  echo $cmd
  eval $cmd
  echo '------------------------------------------------------------------------'
done