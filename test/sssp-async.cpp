#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <queue>
using namespace std;

double d = 0.85;
double threshold = 1e-8;

struct Node
{
    int to;
    int weight;
    Node(int to, int weight) : to(to), weight(weight) {}
};

// g++ sssp.cpp && ./a.out
int main(int argc, char **argv)
{
    // double start_program_time = clock();

    string file_ = argv[1];
    // string file_ = "../dataset/roadNet-CA.txt.m";
    ifstream infile;
    infile.open(file_);
    if (!infile)
    {
        cout << "open file " << file_ << " failed." << endl;
        return 0;
    }
    vector<vector<Node>> neib_vec;
    unsigned int n = 0, m = 0, source = 0;
    int u, v, w=1;
    while (infile >> u >> v)
    {
        m++;
        if (neib_vec.size() < max(u, v) + 1)
            neib_vec.resize(max(u, v) + 1);
        Node node(v, w);
        neib_vec[u].emplace_back(node);
    }
    infile.close();
    n = neib_vec.size();
    cout << "n = " << n << ", m = " << m << endl;

    // 实现csr存储
    vector<unsigned int> csr;       // 长度为m
    vector<unsigned int> csr_w;     // 长度为m
    vector<unsigned int> csr_index; // 长度为n+1，其中csr_index[n]=m
    unsigned int curr_index = 0;
    for (int i = 0; i < n; i++)
    {
        csr_index.emplace_back(curr_index);
        unsigned int curr_neib_num = neib_vec[i].size();
        for (int j = 0; j < curr_neib_num; j++)
        {
            csr.emplace_back(neib_vec[i][j].to);
            csr_w.emplace_back(neib_vec[i][j].weight);
        }
        curr_index += curr_neib_num;
    }
    csr_index.emplace_back(m);

    long long calc_amount = 0;
    int cnt = 0; // 统计迭代几轮
    const int MAX = 9999999;
    double start_iter_time = clock();
    vector<double> dist(n, MAX);
    // vector<double> delta(n, MAX);
    bool is_continue = true;
    dist[source] = 0;
// cout << "======以下是每轮迭代用时=====\n";
    // 如果原点到某个点的距离更新了，通过这个点到达其他点的最短距离 也应该更新。就再进行循环以更新
    while (is_continue)
    {
        // double now_time = clock();
        is_continue = false;
        for (int i = 0; i < n; i++)
        {
            int start_index = csr_index[i];
            int end_index = csr_index[i + 1];
            for (int j = start_index; j < end_index; j++)
            {
                int neib_idx = csr[j];
                if (dist[i] + csr_w[j] < dist[neib_idx])
                {
                    dist[neib_idx] = dist[i] + csr_w[j];
                    is_continue = true; // 如果有点的距离更新了，就需要再进行循环更新其他点的距离
                    // cout << "vtx" << neib_idx << " dist changes\n";
                    calc_amount++;
                }
            }
        }
        cnt++;
        // cout << (clock() - now_time)/CLOCKS_PER_SEC << endl;
    }
// cout << "=====每轮迭代时间输出完毕=====\n";
    cout << "迭代轮数：" << cnt << endl;
    cout << "计算次数：" << calc_amount << endl;
    cout << "计算时间：" << (clock() - start_iter_time) / CLOCKS_PER_SEC << endl;
    // cout << "程序运行总时间：" << (clock() - start_program_time) / CLOCKS_PER_SEC << endl;

    return 0;
}