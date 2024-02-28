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

int main(int argc, char **argv)
{
    clock_t start = clock();
    string filename = argv[1]; 
    ifstream infile(filename);
    string line;

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
    clock_t time1 = clock();
    vector<int> sortedVertices(vertices.begin(), vertices.end());
    sort(sortedVertices.begin(), sortedVertices.end());

    std::map<int, int> vertexMap;
    int index = 0;
    for (int vertex : sortedVertices)
    {
        vertexMap[vertex] = index;
        ++index;
    }

    ofstream outfile(filename + ".m");
    vector<vector<int>> neibs(index);
#pragma omp parallel for
    for (auto p : edges)
    {
        int mapped_u = vertexMap[p.first];
        int mapped_v = vertexMap[p.second];
#pragma omp critical
        outfile << mapped_u << ' ' << mapped_v << '\n';
#pragma omp critical
        neibs[mapped_u].emplace_back(mapped_v);
    }
    int zero_outdeg_vtx = 0;
    for (int i = 0; i < index; i++)
    {
        if (neibs[i].size() == 0)
        {
            ++zero_outdeg_vtx;
            random_device rd;                            
            mt19937 gen(rd());                            
            uniform_int_distribution<> dis(0, index - 1); 
            int random_vtx = dis(gen);              
            outfile << i << ' ' << random_vtx << '\n';
        }
    }
    outfile.close();
    clock_t time4 = clock();
    cout << "Program execution time: " << (double)(clock() - start) / CLOCKS_PER_SEC << " s" << std::endl;
    return 0;
}