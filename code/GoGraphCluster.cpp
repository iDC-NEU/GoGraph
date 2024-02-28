#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <stack>
#include <map>
#include <queue>
#include <algorithm>
#include <utility>
#include "omp.h"
#include "./utils/Psort.h"
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstring>

#define MIN_SCORE -2147483646
double d = 0.85;
double threshold = 1e-6;

bool cmp(std::pair<int, int> a, std::pair<int, int> b)
{
    return a.second > b.second;
}

bool cmp2(const std::pair<int, int>& a, const std::pair<int, int>& b) {
    return a.second > b.second;
}



struct ordered_vertex{
    int id;
    double score;
    int type; 
};

struct cluster_vertex{
    int id;
    int is_BFS_visit;

};

struct vertex_score_info{
    int id;
    double score;
};

struct cluster_info{
    int id;
    double score;
};


struct cluster_neighbor{
    int id;
    double score;
    int type;
};

bool cmp_v_score(ordered_vertex a, ordered_vertex b){
    return a.score<b.score;
}

bool cmp_cluster_score(cluster_neighbor a , cluster_neighbor b){
    return a.score<b.score;

}

bool cmp_vertex_score_info(vertex_score_info a, vertex_score_info b){
    return a.score<b.score;

}

bool cmp_cluster_info(cluster_info a , cluster_info b){
    return a.score<b.score;

}


bool if_in_vec(std::vector<int> vec , int element){
    for(int i = 0;i<vec.size();i++){
        if(vec[i]==element){return true;}
    }
    return false;
}


std::vector<int> BFS(std::vector<std::vector<int>> out_neib_vec,std::vector<int> belong_to_cluster,int root,int cluster_id,std::vector<cluster_vertex> cluster, std::map<int,int> inner_vertex_index){

    std::vector<int> BFS_result;

    std::queue<int> BFS_queue;

    //std::vector<int> visited;

    BFS_queue.push(root);

    //visited.push_back(root);

    cluster[inner_vertex_index[root]].is_BFS_visit=1;

    //std::cout<<"start BFS"<<std::endl;
    //BFS
    while(!BFS_queue.empty()){

        int curr_vtx = BFS_queue.front();
        BFS_result.push_back(curr_vtx);
        //visited.push_back(curr_vtx);
        BFS_queue.pop();
        for(int i=0 ; i < out_neib_vec[curr_vtx].size() ; i++){
            if( belong_to_cluster[out_neib_vec[curr_vtx][i]] == cluster_id && cluster[inner_vertex_index[out_neib_vec[curr_vtx][i]]].is_BFS_visit==0){
                
                BFS_queue.push(out_neib_vec[curr_vtx][i]);
                
                cluster[inner_vertex_index[out_neib_vec[curr_vtx][i]]].is_BFS_visit=1; 

            }
        }

    }

    for(int i=0;i<cluster.size();i++){
        if(cluster[i].is_BFS_visit==0){
            
            BFS_result.push_back(cluster[i].id);
        }

    }
    //std::cout<<"finished BFS"<<std::endl;
    return BFS_result;

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



void getMaxScoreForCluster(std::vector<cluster_neighbor> neighbor, std::vector<std::vector<int>> cluster_edge, double &Maxscore,int cluster_id){
    if(neighbor.size()==0){
        Maxscore = -100;
        return;
    }

    double max_score = 0;
    int max_index = -1;

    double score = 0;

    for(int i=0;i<neighbor.size();i++){
        
        if(neighbor[i].type==1){
            score += cluster_edge[neighbor[i].id][cluster_id];
            if(score>max_score){
                max_score = score;
                max_index = i;
            }

        }

        else if(neighbor[i].type==2){
            score -= cluster_edge[cluster_id][neighbor[i].id];
            if(score>max_score){
                max_score = score;
                max_index = i;
            }

        }

    }

    if(max_index==-1){
        Maxscore = neighbor[0].score - 5;
    }

    else if(max_index == neighbor.size()-1){
        Maxscore = neighbor[neighbor.size()-1].score+5;

    }
    else{
        Maxscore = (neighbor[max_index].score + neighbor[max_index+1].score)/2;
    }

}





int findMaxRootScore(std::vector<std::pair<int, int>> inner_vtx,
                     int& root_ptr, std::vector<bool>& is_visited)
{
    int n = inner_vtx.size();
    int max_score = MIN_SCORE;
    int root = -1;
    int i = root_ptr;
    for ( ; i < n; i++)
    {
        int curr_vtx = inner_vtx[i].first;
        if(!is_visited[curr_vtx] && inner_vtx[i].second > max_score)
        {
            max_score = inner_vtx[i].second;
            root = inner_vtx[i].first;
            is_visited[root] = true;
            break;
        }
    }
    root_ptr = ++i;
    return root;
}



void getQ(int root, std::vector<bool>& is_visited, std::vector<std::vector<int>>& out_neib_vec,
          std::unordered_map<int, int> map_id_score, std::queue<int> &neib_q)
{
    std::vector<std::pair<int, int>> neib_score;
    if(out_neib_vec[root].size() > 0)
    {
        for(auto u: out_neib_vec[root])
        {
            if(is_visited[u]) continue;
            neib_score.emplace_back(std::make_pair(u, map_id_score[u]));
        }
        if(neib_score.size() > 0)
        {
            sort(neib_score.begin(), neib_score.end(), cmp2);
            for(auto u: neib_score)
            {
                neib_q.push(u.first);
                is_visited[u.first] = true;
            }
        }
    }
}



int main(int argc, char** argv){

    // read file
    double load_time = clock();
    std::string input_filename = argv[1];
    std::ifstream infile(input_filename);
    std::vector<std::pair<int, int> > edges;
    int max_vid = -1;
    int u, v;
    while (infile >> u >> v) {
        edges.emplace_back(std::make_pair(u, v));
        max_vid = max_vid < u ? u : max_vid;
        max_vid = max_vid < v ? v : max_vid;
    }
    infile.close();

    int n = max_vid + 1;
    std::cout << "vertices num = " << n << std::endl;

    std::vector<std::vector<int>> out_neib_vec(n);
    std::vector<std::vector<int>> in_neib_vec(n);




    for (auto edge : edges) {
        u = edge.first;
        v = edge.second;

        out_neib_vec[u].emplace_back(v);
        in_neib_vec[v].emplace_back(u);
    }


    std::string cluster_filename = argv[2];
    std::vector<std::vector<cluster_vertex>> id_and_cluster;
    std::vector<std::map<int,int>> cluster_inner_index;

    infile.open(cluster_filename);
    std::string line;
    int cluster_num = 0;
    std::vector<int> belong_to_cluster(n);
    while (getline(infile, line))
    {
        int count;
        //std::vector<int> row;
        std::vector<cluster_vertex> cluster_vertex_temp;
        std::map<int,int> cluster_inner_index_temp;
        std::istringstream iss(line);
        iss >> count;
        int number;
        int index=0;
        while (iss >> number && cluster_vertex_temp.size() < count) { 
            cluster_vertex_temp.push_back({number,0});
            cluster_inner_index_temp[number] = index;
            index++;

            belong_to_cluster[number] = cluster_num;
        }

        id_and_cluster.emplace_back(cluster_vertex_temp);
        cluster_inner_index.push_back(cluster_inner_index_temp);

        ++cluster_num;
    }
    infile.close();

    std::cout << "The total number of clusters is " << cluster_num << std::endl;

    std::cout << "#Load dataset time: "
         << (clock() - load_time) / CLOCKS_PER_SEC << std::endl;



    std::vector<std::vector<vertex_score_info>> all_vertex_score;

    double vertex_process_start = clock();
    for(int i=0 ; i<cluster_num ; i++){

        std::vector<int> vertex_queue = BFS(out_neib_vec , belong_to_cluster , id_and_cluster[i][rand()%id_and_cluster[i].size()].id , i, id_and_cluster[i] , cluster_inner_index[i]);//i就是cluster值

        std::unordered_map<int, double> vertex_score;

        for(auto v : id_and_cluster[i]){
            vertex_score[v.id] = MIN_SCORE;
        }


        for(int j=0;j<vertex_queue.size();j++){
            if(j==0){
                vertex_score[vertex_queue[j]] = 1;
                continue;
            }

            std::vector<ordered_vertex> neighbor_vertex;

            for(auto out_neighbor: out_neib_vec[vertex_queue[j]]){
                if(belong_to_cluster[out_neighbor] == i){
                    if(vertex_score[out_neighbor]!=MIN_SCORE){
                        neighbor_vertex.push_back({out_neighbor,vertex_score[out_neighbor], 2});
                    }
                }
            }

            for(auto in_neighbor: in_neib_vec[vertex_queue[j]]){
                if(belong_to_cluster[in_neighbor] == i){
                    if(vertex_score[in_neighbor]!=MIN_SCORE){
                        neighbor_vertex.push_back({in_neighbor,vertex_score[in_neighbor], 1});
                    }
                }
            }

            std::sort(neighbor_vertex.begin() , neighbor_vertex.end() , cmp_v_score);

            double temp_maxscore = 0;
            getMaxScore(neighbor_vertex , temp_maxscore);
            vertex_score[vertex_queue[j]] = temp_maxscore;
        }


        std::vector<vertex_score_info> cluster_vertex_score;
        for(auto vs : vertex_score){
            cluster_vertex_score.push_back({vs.first,vs.second});
        }

        std::sort(cluster_vertex_score.begin(),cluster_vertex_score.end(), cmp_vertex_score_info);



        all_vertex_score.push_back(cluster_vertex_score);

    }


    std::cout<<"finish inner vertex!"<<std::endl;



    std::vector<std::vector<int>> cluster_edge(cluster_num, std::vector<int>(cluster_num, 0));

    std::vector<cluster_info> cluster_and_score;

    for(int i=0;i<cluster_num;i++){
        for(auto curr_vertex : id_and_cluster[i]){
            for(auto out_vertex : out_neib_vec[curr_vertex.id]){
                if(belong_to_cluster[out_vertex] != i){
                    cluster_edge[i][belong_to_cluster[out_vertex]]++;
                }
            }
        }

        cluster_and_score.push_back({i,-9999999}); 


    }





   for(int i=0;i<cluster_num;i++){

       if(i==0){
           cluster_and_score[i].score=1;
           continue;
       }

       std::vector<cluster_neighbor> neighbor;

       for(int j=0;j<cluster_num;j++){
           if(j!=i){
               if(cluster_edge[i][j]>0 && cluster_and_score[j].score > -9999999){
                   neighbor.push_back({j,cluster_and_score[j].score,2});
               }
           }

       }

       for(int k=0;k<cluster_num;k++){
           if(k!=i){
               if(cluster_edge[k][i]>0 && cluster_and_score[k].score > -9999999){
                   neighbor.push_back({k,cluster_and_score[k].score,1});

               }

           }

       }

       std::sort(neighbor.begin(),neighbor.end(), cmp_cluster_score);

       double maxScore = 0;
       getMaxScoreForCluster(neighbor , cluster_edge , maxScore,i);

       cluster_and_score[i].score = maxScore;

   }

   std::sort(cluster_and_score.begin(), cluster_and_score.end(), cmp_cluster_info);


    std::cout<<"process time: "<<(clock() - vertex_process_start)/ CLOCKS_PER_SEC<<std::endl;




   std::ofstream outfile(input_filename+".gographcluster_v");

    outfile.setf(std::ios::fixed);
    outfile.precision(3);

    double current_score = -500000;

    for(int i=0;i<cluster_and_score.size();i++){

        int current_cluster = cluster_and_score[i].id;

        for(int j=0;j<all_vertex_score[current_cluster].size();j++){
            outfile << all_vertex_score[current_cluster][j].id<<" "<< current_score<< '\n';
            current_score += 1;
        }

    }
    outfile.close();
    return 0;
}