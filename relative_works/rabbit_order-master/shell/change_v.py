# 转化Rabbit的输出结果:
#   rabbit:
#      old_id -> new_id:
#         0 -> 3
#         1 -> 2
#         2 -> 0
#         3 -> 1
#   change to:
#         0 <- 2
#         1 <- 3
#         2 <- 1
#         3 <- 3
#

files=['uk-2002'] #,'web-indochina-2004', 'sk-2005', 'Cit-HepPh', 'web-edu', 'web-polblogs', 'cit-Patents']

for filename in files:
  path = r'/home/yusong/dataset/large/' + filename + '/'+ filename + '.mtx.rabbit_v'
  # path = '../dataset/test_change.v'
  print(path)
  with open(path, 'r+') as f:
    old_ids = f.readlines() #.strip('\n').split('\n')
  new_ids_map = {}
  for i in range(len(old_ids)):
    new_ids_map[int(old_ids[i])] = i
  new_ids_map = sorted(new_ids_map.items(), key=lambda x:x[0])
  with open(path + '_new', 'w+') as f:
    for k,v in new_ids_map:
      f.write(str(v) + '\n')
