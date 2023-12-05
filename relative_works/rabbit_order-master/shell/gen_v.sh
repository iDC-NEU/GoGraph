path=/home/yusong/code/rabbit_order-master/demo
files=(uk-2002) #web-indochina-2004 sk-2005 Cit-HepPh web-edu web-polblogs cit-Patents)

for filename in ${files[@]}
do
  efile="/home/yusong/dataset/large/${filename}/${filename}.mtx"
  ls $efile
  ${path}/reorder ${efile} > ${efile}.rabbit_v
done