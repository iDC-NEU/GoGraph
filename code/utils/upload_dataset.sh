#!/bin/bash

# 定义目标服务器的IP地址和登录用户
target_host="219.216.65.104"
user="yusong"

files=(web-indochina-2004 sk-2005 google cit-Patents wikilink)
algs=(GoGraph OnlyScore) #Origin

for filename in ${files[@]}
do
    for algname in ${algs[@]}
    do
        # 定义要传输的本地文件列表
        file_list=(
            # "/home/liupengxi/dataset/large/${filename}/${filename}.mtx.reorder.v"
            # "/home/liupengxi/dataset/large/${filename}/${filename}.mtx.reorder.e"
            "/home/liupengxi/dataset/large/${filename}/${filename}.mtx.reorder.e.${algname}_v"
        )

    # 循环遍历文件列表，使用scp命令传输文件到目标服务器
    for file in ${file_list[@]}
    do
        scp ${file} ${user}@${target_host}:~/dataset/large/${filename}/
    done
    done
done