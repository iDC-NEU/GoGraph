# ##############################################################################
root_path=/home/liupengxi/code/GoGraph
files=("$1") # web-Google roadNet-CA soc-twitter-2010     # web-indochina-2004 sk-2005 google cit-Patents wikilink

cd $root_path
g++ pretreatment.cpp -std=c++11 -o pretreatment

for filename in ${files[@]}
do
  echo -e "\n#pretreatment" 
  efile="/home/liupengxi/dataset/large/${filename}/${filename}.mtx"
  cmd="${root_path}/pretreatment ${efile}"
  echo $cmd
  eval $cmd
  echo '------------------------------------------------------------------------'
done