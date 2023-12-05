#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <unordered_map>
using namespace std;

int main(int argc, char** argv) {
    int u;
    double v;
    string input_filename_1 = argv[1];
    ifstream infile_1(input_filename_1);
    vector<pair<int, double> > reorder_vexter;
    while (infile_1 >> u >> v) {
        reorder_vexter.emplace_back(make_pair(u,v));
    }
    infile_1.close();

    string input_filename_2 = argv[2];
    ifstream infile_2(input_filename_2);
    unordered_map<int, int> vexter_mapping;
    while (infile_2 >> u >> v) {
        vexter_mapping.insert(make_pair(u,v));
    }
    infile_2.close();

    std::ofstream f_v(input_filename_1 + ".mid.v");

    f_v.setf(std::ios::fixed);
    f_v.precision(3);//精度为输出小数点后3位
    
    for (int i = 0 ; i < reorder_vexter.size();i++) {
        int key = reorder_vexter[i].first;
        auto it = vexter_mapping.find(key);
        if (it != vexter_mapping.end()) {
            f_v << it->second << " " <<reorder_vexter[i].second <<endl;
        } else {
            f_v << key << " is not found in the map" << endl;
        }
    }
    f_v.close();

    // cmd="${root_path}/PartVexter 新生成id顶点文件 新id和原id的映射文件"
}