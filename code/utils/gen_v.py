########### 前六个数据集
# node_num = [11358, 121422, 34546, 3031, 643, 3774768]
# i = 0
# for filename in ['web-indochina-2004', 'sk-2005', 'Cit-HepPh', 'web-edu', 'web-polblogs', 'cit-Patents']:
#   efile = "/home/yusong/dataset/large/" + filename + "/" + filename + ".mtx.new"
#   print("i=", i)
#   print('efile: ', efile, " node_num=", node_num[i])
#   with open(efile + '.v', 'w+') as f:
#     for j in range(node_num[i]):
#       f.write(str(j) + "\n")
#   i += 1


# google, uk-2002: 875713, 18484117
# 
node_num = [420265, 83832, 23947347, 4906578, ]
i = 0
for filename in ['ljournal-2008', 'hollywood-2009', 'road-usa', 'wikilink', 'big-sk-2005']:
  efile = "/home/yusong/dataset/large/" + filename + "/" + filename + ".mtx.new"
  print("i=", i)
  print('efile: ', efile, " node_num=", node_num[i])
  with open(efile + '.v', 'w+') as f:
    for j in range(node_num[i]):
      f.write(str(j) + "\n")
  i += 1
  echo '------------------------------------------------------------------------'