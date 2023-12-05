# ##############################################################################
root_path=/home/liupengxi/code/GoGraph
files=("$1") # web-Google roadNet-CA soc-twitter-2010     # web-sk-2005-2004 sk-2005 google cit-Patents wikilink

cd $root_path
g++ changetxt.cpp -std=c++11 -o changetxt

for filename in ${files[@]}
do
  echo -e "\n#changetxt" 
  efile="/home/liupengxi/dataset/large/${filename}/${filename}.mtx"
  cmd="${root_path}/changetxt ${efile}"
  echo $cmd
  eval $cmd
  echo '------------------------------------------------------------------------'
done