#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <stack>
#include <map>
#include <queue>
#include <algorithm>
#include <math.h>
#include <stdlib.h>
using namespace std;

bool cmp(int a, int b, const map<int, int>& m) {
    return m.at(a) < m.at(b);
}

struct ordered_vertex{
    int id;
    double score;
    int type;
};

struct Vertex{
    int id;
    double score;
};

bool cmp_Vertex(Vertex a, Vertex b){
    return a.score<b.score;
}


bool cmp_neighbor(ordered_vertex a , ordered_vertex b){
    return a.score<b.score;
}


void getMaxScore(std::vector<ordered_vertex> neighbor_vertex, double& Maxscore){

    if(neighbor_vertex.size()==0){
        Maxscore=-100;
        return;
    }

    double max_score = 0;
    int max_index = -1;

    double score = 0;

    for(int i=0 ; i< neighbor_vertex.size() ; i++){
        if(neighbor_vertex[i].type==1){
            score++;
            if(score > max_score){
                max_score = score;
                max_index = i;
            }
        }
        else if(neighbor_vertex[i].type == 2){
            score--;
            if(score > max_score){
                max_score = score;
                max_index = i;
            }
        }


    }



    if(max_index==-1){
        Maxscore = neighbor_vertex[0].score - 5;
    }

    else if(max_index == neighbor_vertex.size()-1){
        Maxscore = neighbor_vertex[neighbor_vertex.size()-1].score + 5;

    }
    else{
        Maxscore = (neighbor_vertex[max_index].score + neighbor_vertex[max_index+1].score)/2;
    }

}




int main(int argc, char** argv) {
    int MAX_SCORE = -2147483648;
    double load_time = clock();
    string input_filename = argv[1];
    ifstream infile(input_filename);
    vector<pair<int, int> > edges;
    int max_vid = -1;
    int u, v;
    while (infile >> u >> v) {
        edges.emplace_back(make_pair(u, v));
        max_vid = max_vid < u ? u : max_vid;
        max_vid = max_vid < v ? v : max_vid;
    }
    infile.close();

    string input_filename_1 = argv[2];
    ifstream infile_1(input_filename_1);
    vector<int> isolated_points;

    while (infile_1 >> v) {
        isolated_points.emplace_back(v);
    }
    infile_1.close();

    string input_filename_2 = argv[3];
    ifstream infile_2(input_filename_2);

    int v_id;
    double v_score;

    vector<Vertex> all_vertex;

    while (infile_2 >> v_id >> v_score){
        all_vertex.push_back({v_id , v_score});
    }

    infile_2.close();



    int n = max_vid + 1;
    cout << "node_num = " << n << endl;
    vector<vector<int>> out_neib_vec(n);
    vector<vector<int>> in_neib_vec(n);
    for (auto edge : edges) {
        u = edge.first;
        v = edge.second;
        out_neib_vec[u].emplace_back(v);
        in_neib_vec[v].emplace_back(u);
    }



    map<int,int> index_of_all_Vertex;



    for(int i=0;i<isolated_points.size();i++){
        all_vertex.push_back({isolated_points[i] , -9999999});
    }

    for(int i=0;i<all_vertex.size();i++){
        index_of_all_Vertex[all_vertex[i].id] = i;
    }



    for(int i=0;i<isolated_points.size();i++){

        vector<ordered_vertex>neighbor;

        for(auto out_vertex: out_neib_vec[isolated_points[i]]){
            if(all_vertex[index_of_all_Vertex[out_vertex]].score!=-9999999){
                neighbor.push_back({out_vertex,all_vertex[index_of_all_Vertex[out_vertex]].score,2});

            }
        }
        for(auto in_vertex : in_neib_vec[isolated_points[i]]){
            if(all_vertex[index_of_all_Vertex[in_vertex]].score!=-9999999){
                neighbor.push_back({in_vertex,all_vertex[index_of_all_Vertex[in_vertex]].score, 1});

            }

        }

        sort(neighbor.begin() , neighbor.end() , cmp_neighbor);


        double Maxscore = 0;
        getMaxScore(neighbor,Maxscore);

        all_vertex[index_of_all_Vertex[isolated_points[i]]].score = Maxscore;


    }


    std::sort(all_vertex.begin(), all_vertex.end(), cmp_Vertex);





    std::ofstream outfile(input_filename + ".GoGraph_v");

    for(int i=0;i<all_vertex.size();i++){
        outfile << all_vertex[i].id << '\n';

    }


    outfile.close();

    return 0;
}
