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
#include <omp.h>
using namespace std;

bool cmp(pair<int, int> a, pair<int, int> b)
{
    return a.second > b.second;
}

int main(int argc, char **argv)
{
    clock_t start = clock();
    string filename = argv[1];
    ifstream infile;
    infile.open(filename);
    vector<pair<int, int>> edges;
    edges.reserve(271507470 + 1);
    int u, v;
    int max_id = 0;
    while (infile >> u >> v)
    {
        edges.emplace_back(make_pair(u, v));
        max_id = max(max_id, u);
        max_id = max(max_id, v);
    }
    infile.close();
    // cout << "edges.size() = " << edges.size() << endl;
    int n = max_id + 1;
    cout << "vertex num = " << n << ", ";

    vector<vector<int>> out_neib_vec(n);
    vector<vector<int>> in_neib_vec(n);
    vector<vector<int>> all_neib_vec(n);
    for (auto edge : edges)
    {
        int u = edge.first;
        int v = edge.second;
        out_neib_vec[u].emplace_back(v);
        in_neib_vec[v].emplace_back(u);
        all_neib_vec[u].emplace_back(v);
        all_neib_vec[v].emplace_back(u);
    }
    clock_t time2 = clock();



    vector<int> flag(n, 0);
    double threshold = 0.002;
    int n1 = n * threshold;
    vector<pair<int, int>> idx_totaldeg_pair(n);
    for (int i = 0; i < n; i++) {
        int totaldeg = out_neib_vec[i].size() + in_neib_vec[i].size();
        idx_totaldeg_pair[i] = make_pair(i, totaldeg);
    }

    sort(idx_totaldeg_pair.begin(), idx_totaldeg_pair.end(), cmp);

    vector<int> bigdeg;
    bigdeg.reserve(n1);
    int num_bigdeg = 0;

    #pragma omp parallel for
    for (int i = 0; i < n1; i++) {
        bigdeg.emplace_back(idx_totaldeg_pair[i].first);
        flag[idx_totaldeg_pair[i].first] = 1; // 
        num_bigdeg++;
    }

    ofstream outfile;
    outfile.open(filename + ".big");
    for (auto p : bigdeg) {
        outfile << p << '\n';
    }
    outfile.close();



    clock_t time3 = clock();

    int num_mid = 0;
    vector<int> map_old_new(n, 0);
    vector<int> map_new_old(n, 0);
    for (int i = 0; i < n; i++)
    {
        if (flag[i] == 1)
            continue;
        for (auto v : all_neib_vec[i])
        {
            if (flag[v] != 1)
            {
                flag[i] = 2;
                map_old_new[i] = num_mid;
                map_new_old[num_mid] = i;
                num_mid++;
                break;
            }
        }
    }

    outfile.open(filename + ".v.map");
    for (int i = 0; i < num_mid; i++)
    {
        outfile << i << " " <<map_new_old[i] << endl;
    }
    outfile.close();

    {
        ofstream outfile(filename + ".b.e", ios::out | ios::binary);
        char *edge_buffer = new char[edges.size() * sizeof(int) * 2];
        int offset = 0;
        int edge_num = 0;
        for (auto &edge : edges)
        {
            int u = edge.first;
            int v = edge.second;
            if (flag[u] == 2 && flag[v] == 2)
            {
                *(int *)(edge_buffer + offset) = map_old_new[u];
                offset += sizeof(int);
                *(int *)(edge_buffer + offset) = map_old_new[v];
                offset += sizeof(int);
                edge_num++;
            }
        }
        cout << "edge num = " << edge_num << endl;
        outfile.write(edge_buffer, offset);
        delete[] edge_buffer;
        outfile.close();
    }
    clock_t time4 = clock();

    int num_iso = 0;
    outfile.open(filename + ".iso");
    for (int i = 0; i < n; i++)
    {
        if (flag[i] == 0)
        {
            outfile << i << endl;
            num_iso++;
        }
    }
    outfile.close();


    //------------------------------------------------------------------------------------------------------------------------

    string binaryFileName = filename + ".b.e"; 
    string txtFileName = filename + ".mid.e"; 

    ifstream binaryFile(binaryFileName, ios::binary);

    if (!binaryFile.is_open()) {
        cerr << "Error opening binary file." << endl;
        return 1;
    }

    ofstream txtFile(txtFileName);

    if (!txtFile.is_open()) {
        cerr << "Error opening text file." << endl;
        binaryFile.close();
        return 1;
    }

    while (binaryFile.read(reinterpret_cast<char*>(&u), sizeof(int)) &&
           binaryFile.read(reinterpret_cast<char*>(&v), sizeof(int))) {
        txtFile << u << " " << v << endl;
    }

    binaryFile.close();
    txtFile.close();

    //------------------------------------------------------------------------------------------------------------------------

    cout << "Pre-processing time: " << (double)(clock() - start) / CLOCKS_PER_SEC << " s" << endl;
    return 0;
}
