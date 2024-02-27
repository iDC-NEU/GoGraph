//输入：一个数据集文件
//目标：顶点下标从0开始，每个顶点都有出度
//输出：.m文件，第一行是顶点个数，随后每一行是边

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <unordered_set>
#include <map>
#include <algorithm>
#include <random>
#include <omp.h> // 引入OpenMP头文件
using namespace std;

int main(int argc, char **argv)
{
    clock_t start = clock();
    string filename = argv[1]; //使用 /home/user/zhouyj/dataset/web-Google.mtx作为例子
    ifstream infile(filename);
    string line;

    // while (getline(infile, line))
    // {
    //     if (line[0] != '#')
    //         break;
    // }

    //跳过前2行
    // for(int i = 0; i < 2; i++)
    // {
    //     getline(infile, line);
    // }

    int u, v;
    vector<pair<int, int>> edges;
    unordered_set<int> vertices;
    while (infile >> u >> v)
    {
        vertices.insert(u);
        vertices.insert(v);
        edges.emplace_back(make_pair(u, v));
    }
    infile.close();
    // cout << "edges.size() = " << edges.size() << endl;
    clock_t time1 = clock();
    // cout << "读文件，用时：" << (double)(time1 - start) / CLOCKS_PER_SEC << endl;
    // cout << "vertices.size() = " << vertices.size() << endl;
    // 对顶点进行排序
    vector<int> sortedVertices(vertices.begin(), vertices.end());
    sort(sortedVertices.begin(), sortedVertices.end());

    // 创建映射表
    std::map<int, int> vertexMap;
    int index = 0;
    for (int vertex : sortedVertices)
    {
        vertexMap[vertex] = index;
        ++index;
    }
    // cout << "n = " << index << endl;
    clock_t time2 = clock();
    // cout << "排序顶点，创建映射表，用时：" << (double)(time2 - time1) / CLOCKS_PER_SEC << endl;

    // 重新读取边文件并进行顶点映射
    // 如果有出度为0的顶点，就令其随机指向一个顶点，范围[0,index)
    ofstream outfile(filename + ".m");
    // outfile << vertices.size() << endl; // 第一行为顶点个数
    vector<vector<int>> neibs(index);
#pragma omp parallel for
    for (auto p : edges) // edges里面存的是从u到v的一条边
    {
        int mapped_u = vertexMap[p.first];
        int mapped_v = vertexMap[p.second];
#pragma omp critical
        outfile << mapped_u << ' ' << mapped_v << '\n';
#pragma omp critical
        neibs[mapped_u].emplace_back(mapped_v);
    }
    clock_t time3 = clock();
    // cout << "遍历边向量，把映射完的边存起来，用时：" << (double)(time3 - time2) / CLOCKS_PER_SEC << endl;
    // while (getline(infile, line))
    // {
    //     if (line.empty() || line[0] == '#')
    //         continue;
    //     istringstream iss(line);
    //     int u, v;
    //     if (iss >> u >> v)
    //     {
    //         int mapped_u = vertexMap[u];
    //         int mapped_v = vertexMap[v];
    //         outfile <<  mapped_u << ' ' << mapped_v << '\n';
    //         neibs[mapped_u].emplace_back(mapped_v);
    //     }
    // }
    // infile.close();
    int zero_outdeg_vtx = 0;
    for (int i = 0; i < index; i++)
    {
        if (neibs[i].size() == 0) // 随机生成一个[0,index)的顶点
        {
            ++zero_outdeg_vtx;
            random_device rd;                             // 随机设备作为种子
            mt19937 gen(rd());                            // 梅森旋转算法作为随机数生成器
            uniform_int_distribution<> dis(0, index - 1); // 均匀分布生成 [0, n) 的整数
            int random_vtx = dis(gen);                    // 生成随机整数
            // neibs[i].emplace_back(random_vtx);
            outfile << i << ' ' << random_vtx << '\n';
        }
    }
    outfile.close();
    clock_t time4 = clock();
    // cout << "使0出度顶点随机指向另外的顶点，写文件，用时：" << (double)(time4 - time3) / CLOCKS_PER_SEC << endl;
    cout << "Program execution time: " << (double)(clock() - start) / CLOCKS_PER_SEC << " s" << std::endl;
    return 0;
}