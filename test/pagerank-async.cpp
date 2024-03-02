#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <ctime>
using namespace std;
int main(int argc, char** argv)
{
    double d = 0.85;
    double threshold = 1e-8;
    cout << "argc = " << argc << endl;

    
    string edge_file = argv[1];
    ifstream infile;
    infile.open(edge_file);
    int u, v;
    unordered_set<int> vertices;
    while(infile >> u >> v)
    {
        vertices.insert(u);
        vertices.insert(v);
    }
    infile.close();
    int n = vertices.size();
    cout << "n = " << n << endl;

    vector<vector<int> > edges(n);
    infile.open(edge_file);
    while (infile >> u >> v)
    {
        edges[u].emplace_back(v);
    }
    infile.close();
        
    vector<double> value(n, 0);
    vector<double> delta(n, 1-d);
    int iter_count = 0;
    bool is_continue = true;
        
    double start_time = clock();
    while(is_continue)
    { 
        is_continue = false;
        for(int i = 0; i < n; i++)
        {
            if(delta[i] > threshold)
                is_continue = true;

            value[i] += delta[i];
            double prop_delta = delta[i] * d / edges[i].size();
            delta[i] = 0;

            for (auto j : edges[i])
            {
                delta[j] += prop_delta;
            }
        }

        iter_count++;
            
    }
    double sum = 0;
    for(int i = 0; i < n; i++)
    {
        sum += value[i];
    }
    cout << "check: sum = " << sum / n << endl;
    cout << "iter round = " << iter_count << endl;
    cout << "iter time = " << (clock() - start_time)/CLOCKS_PER_SEC << endl;

    return 0;
}