/*
  处理数据集
    - 文本文件与二进制文件相互转换
    - 有向图转为无向图
    - 生成增量数据集
    - 生成带权数据集时，取完模以后需要加上一个正数，防止边权为0
*/

#include <iostream>
#include <fstream>
#include<iomanip>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <gflags/gflags.h>
#include <gflags/gflags_declare.h>
#include <glog/logging.h>
#include <cstdlib>
#include <time.h>
#include <string>
#include <chrono>
#include <set>
#include <map>
using namespace std;
using vid_t = uint64_t;
using w_t = uint64_t;
using edge_num_t = long long; // 需要为有符号整数

struct Edge
{
  vid_t to_vid;
  w_t weight=0;
  
  Edge(){}

  Edge(vid_t to, w_t w) {
    to_vid = to;
    weight = w;
  }

  bool operator==(const Edge b) const {  
    return this->to_vid == b.to_vid && this->to_vid == b.to_vid;  
  }
  bool operator<(const Edge b) const {  
    return this->to_vid < b.to_vid;  
  }
};

struct Edge_two{
  vid_t src_vid;
  vid_t to_vid;
  w_t weight=0;

  Edge_two(){}

  Edge_two(vid_t src, vid_t to, w_t w) {
    src_vid = src;
    to_vid = to;
    weight = w;
  }
};

DEFINE_string(type, "reorder", "gen data way");
DEFINE_string(base_edge, "", "efile path");
DEFINE_string(out_edge, "", "output efile path");
DEFINE_string(out_dir, "", "output efile dir");
DEFINE_bool(weight, false, "Whether the graph has the weight");
DEFINE_bool(hava_inadj, false, "Whether save in-adj of the graph");
DEFINE_bool(directed, false, "Whether is directed graph");
DEFINE_bool(is_random, false, "Whether is random gen dataset");
DEFINE_int32(head, 0, "file head");
DEFINE_int32(worker_num, 0, "worker num");
DEFINE_double(inc_rate, 0, "rate of gen inc file");

auto formatDobleValue(double val, int fixed) {
    auto str = std::to_string(val);
    return str.substr(0, str.find(".") + fixed + 1);
}

class DealGraph{
  public:
  DealGraph() {

  }

  /**
   * 输出图的边, 只输出前100行
  */
  void print_graph() {
    LOG(INFO) << "---------------print graph----------------------";
    for (vid_t i = 0; i < this->node_num; i++) {
      for (auto e : this->graph[i]) {
        LOG(INFO) << "o_adj: " << i << "->" << e.to_vid << " " << e.weight;
      }
      if (FLAGS_hava_inadj) {
        for (auto e : this->graph_inadj[i]) {
          LOG(INFO) << "i_adj: " << i << "->" << e.to_vid << " " << e.weight;
        }
      }
      if (i > 10) {
        LOG(INFO) << " stop print: just print i<10.";
        break;
      }
    }
  }

  /**
   * 读文本文件的图, 没有重编号
  */
  void read_graph_by_txt (const std::string iefile) {
    std::ifstream srcFile( iefile, ios::in );
    if ( !srcFile ) {
      LOG(INFO) << "error opening source: " << iefile;
      exit(0);
    }
    LOG(INFO) << "start read_graph_by_txt: " << iefile;

    char c[10000];
    for (int i = 0; i < FLAGS_head; i++) {
      srcFile.getline(c, 10000);
      LOG(INFO) << "ignore: " << c;
    }

    std::vector<std::pair<vid_t, Edge>> temp_graph;
    vid_t max_vid = 0;
    edge_num_t edge_num = 0;
    vid_t src_id = 0;
    vid_t to_id = 0;
    while ( srcFile >> src_id ) {
      Edge e;
      srcFile >> e.to_vid;
      max_vid = std::max(max_vid, src_id);
      max_vid = std::max(max_vid, e.to_vid);
      if (FLAGS_weight) {
        srcFile >> e.weight;
      }
      temp_graph.emplace_back(std::pair<vid_t, Edge>(src_id, e));
    }
    srcFile.close();

    this->node_num = max_vid + 1;
    this->edge_num = temp_graph.size();
    LOG(INFO) << " node_num=" << this->node_num;
    LOG(INFO) << " edge_num=" << this->edge_num;
    graph.resize(max_vid+1);
    if (FLAGS_hava_inadj) {
      graph_inadj.resize(max_vid+1);
    }
    for (auto p : temp_graph) {
      auto e = p.second;
      // LOG(INFO) << p.first << "->" << e.to_vid << " " << e.weight;
      graph[p.first].emplace_back(e);
      if (FLAGS_hava_inadj) {
        graph_inadj[e.to_vid].emplace_back(Edge(p.first, e.weight));
      }
    }
    LOG(INFO) << "  finish.";
  }

  /**
   * 读文本文件的图, 并重新编号，使得编号连续，即{0, 1, 3, 4} -> {0, 1, 2, 3}.
  */
  void read_graph_by_txt_renum (const std::string iefile, bool undirected = false) {
    std::ifstream srcFile( iefile, ios::in );
    if ( !srcFile ) {
      LOG(INFO) << "error opening source: " << iefile;
      exit(0);
    }
    LOG(INFO) << "start read_graph_by_txt_renum: " << iefile;

    char c[10000];
    for (int i = 0; i < FLAGS_head; i++) {
      srcFile.getline(c, 10000);
      LOG(INFO) << "ignore: " << c;
    }

    std::vector<std::pair<vid_t, Edge>> temp_graph;
    vid_t max_vid = 0;
    edge_num_t edge_num = 0;
    vid_t src_id = 0;
    vid_t to_id = 0;
    std::unordered_map<vid_t, vid_t> v_map;
    vid_t v_id = 0;
    while ( srcFile >> src_id ) {
      Edge e;
      srcFile >> e.to_vid;
      if (v_map.find(src_id) == v_map.end()) {
        v_map[src_id] = v_id;
        src_id = v_id++;
      } else {
        src_id = v_map[src_id];
      }
      if (v_map.find(e.to_vid) == v_map.end()) {
        v_map[e.to_vid] = v_id;
        e.to_vid = v_id++;
      } else {
        e.to_vid = v_map[e.to_vid];
      }
      if (FLAGS_weight) {
        srcFile >> e.weight;
      }
      if (undirected && e.to_vid != src_id) {
        Edge ud_e(src_id, e.to_vid);
        temp_graph.emplace_back(std::pair<vid_t, Edge>(e.to_vid, ud_e));
      }
      temp_graph.emplace_back(std::pair<vid_t, Edge>(src_id, e));
    }
    srcFile.close();

    std::cout << " vid=" << v_id << std::endl;

    //输出原id和新id的映射文件
    std::ofstream f_v(iefile + ".VertexMapping");
    for (auto& kv : v_map) {
        f_v << kv.first << " " << kv.second << std::endl;
    }
    f_v.close();

    this->node_num = v_id;
    this->edge_num = temp_graph.size();
    LOG(INFO) << " node_num=" << this->node_num;
    LOG(INFO) << " edge_num=" << this->edge_num;
    graph.resize(v_id+1);
    if (FLAGS_hava_inadj) {
      graph_inadj.resize(v_id+1);
    }
    for (auto p : temp_graph) {
      auto e = p.second;
      // LOG(INFO) << p.first << "->" << e.to_vid << " " << e.weight;
      graph[p.first].emplace_back(e);
      if (FLAGS_hava_inadj) {
        graph_inadj[e.to_vid].emplace_back(Edge(p.first, e.weight));
      }
    }
    LOG(INFO) << "  finish.";
  }

  /**
   * 读文本文件的图, 并重新编号，使得编号连续，即{0, 1, 3, 4} -> {0, 1, 2, 3}.
  */
  void read_and_write_origin (const std::string iefile, bool undirected = false,
                              const std::string out_edge = "") {
    std::ifstream srcFile( iefile, ios::in );
    if ( !srcFile ) {
      LOG(INFO) << "error opening source: " << iefile;
      exit(0);
    }
    LOG(INFO) << "start read_graph_by_txt_renum: " << iefile;

    char c[10000];
    for (int i = 0; i < FLAGS_head; i++) {
      srcFile.getline(c, 10000);
      LOG(INFO) << "ignore: " << c;
    }

    std::vector<std::pair<vid_t, Edge>> temp_graph;
    vid_t max_vid = 0;
    edge_num_t edge_num = 0;
    vid_t src_id = 0;
    vid_t to_id = 0;
    // std::unordered_map<vid_t, vid_t> v_map;
    std::map<vid_t, vid_t> v_map;
    vid_t v_id = 0;
    // 得到边映射： 保证图结果的前提新进行连续映射
    while ( srcFile >> src_id ) {
      Edge e;
      srcFile >> e.to_vid;
      if (v_map.find(src_id) == v_map.end()) {
        v_map[src_id] = v_id;
        src_id = v_id++;
      } else {
        src_id = v_map[src_id];
      }
      if (v_map.find(e.to_vid) == v_map.end()) {
        v_map[e.to_vid] = v_id;
        e.to_vid = v_id++;
      } else {
        e.to_vid = v_map[e.to_vid];
      }
      if (FLAGS_weight) {
        srcFile >> e.weight;
      }
    }
    srcFile.close();

    std::cout << " vid=" << v_id << std::endl;

    this->node_num = v_id;
    this->edge_num = temp_graph.size();
    LOG(INFO) << " node_num=" << this->node_num;
    LOG(INFO) << " edge_num=" << this->edge_num;

    // for(auto item : v_map) {
    //   LOG(INFO) << item.first << " " << item.second;
    // }
    // 写入点文件
    std::ofstream vFile(out_edge + ".v");
    if ( !vFile ) {
      cout << "error opening destination file. " << out_edge << std::endl;
      vFile.close();
      return;
    }
    for(auto item : v_map) {
      vFile << item.second << "\n";
    }
    vFile.close();
    LOG(INFO) << "  write vfile finish.";

    LOG(INFO) << "  finish.";
  }

  /**
   * 读文本文件的图, 并重新编号，使得编号连续，即{0, 1, 3, 4} -> {0, 1, 2, 3}.
   *    保持原始id的大小顺序, 同时对于没有出度的加出度
    iefile:
      0 6
      1 0
      2 4
      5 7
    oefile:
      0 5
      1 0
      2 3
      4 6
      3 1
      5 4
      6 2
  */
  void read_and_write_keep_order (const std::string iefile, bool undirected = false,
                              const std::string out_edge = "") {
    std::ifstream srcFile( iefile, ios::in );
    if ( !srcFile ) {
      LOG(INFO) << "error opening source: " << iefile;
      exit(0);
    }
    LOG(INFO) << "start read_graph_by_txt_renum: " << iefile;

    char c[10000];
    for (int i = 0; i < FLAGS_head; i++) {
      srcFile.getline(c, 10000);
      LOG(INFO) << "ignore: " << c;
    }

    std::vector<std::pair<vid_t, Edge>> temp_graph;
    vid_t max_vid = 0;
    edge_num_t edge_num = 0;
    vid_t src_id = 0;
    vid_t to_id = 0;
    // std::unordered_map<vid_t, vid_t> v_map;
    std::map<vid_t, vid_t> v_map;
    std::set<vid_t> v_set;
    vid_t v_id = 0;
    // 得到边映射： 保证图结果的前提新进行连续映射
    while ( srcFile >> src_id ) {
      Edge e;
      srcFile >> e.to_vid;
      if (FLAGS_weight) {
        srcFile >> e.weight;
      }
      temp_graph.emplace_back(std::pair<vid_t, Edge>(src_id, e));
      if (undirected && e.to_vid != src_id) {
        Edge ud_e(src_id, e.weight);
        temp_graph.emplace_back(std::pair<vid_t, Edge>(e.to_vid, ud_e));
      }
      v_set.insert(src_id);
      v_set.insert(e.to_vid);
    }
    srcFile.close();

    for (auto v : v_set) {
      v_map[v] = v_id++;
    }

    std::cout << " vid=" << v_id << std::endl;

    this->node_num = v_id;
    this->edge_num = temp_graph.size();
    LOG(INFO) << " node_num=" << this->node_num;
    LOG(INFO) << " edge_num=" << this->edge_num;

    // for(auto item : v_map) {
    //   LOG(INFO) << item.first << " " << item.second;
    // }
    // 写入点文件
    std::ofstream vFile(out_edge + ".v");
    if ( !vFile ) {
      cout << "error opening destination file. " << out_edge << std::endl;
      vFile.close();
      return;
    }
    for(auto item : v_map) {
      vFile << item.second << "\n";
    }
    vFile.close();
    LOG(INFO) << "  write vfile finish. " << (out_edge + ".v");

    /*------------------------------------------------------------------------*/
    // 写入边文件
    const std::string separator=" ";
    LOG(INFO) << "start write_graph: " << out_edge;
    LOG(INFO) << "  separator=" << separator;
    std::ofstream desFile(out_edge + ".e");
    if ( !desFile ) {
      cout << "error opening destination file. " << out_edge << std::endl;
      desFile.close();
      return;
    }
    vid_t add_edges = 0;
    std::unordered_map<vid_t, vid_t> out_degree; // 统计每个点的出度
    // 原始边写入
    for (auto pair : temp_graph) {
      vid_t v = v_map[pair.first];
      vid_t u = v_map[pair.second.to_vid];
      desFile << v << separator;
      desFile << u;
      if (out_degree.find(v) == out_degree.end()) {
        out_degree[v] = 1;
      } else {
        out_degree[v]++;
      }
      if (FLAGS_weight) {
        desFile << separator << pair.second.weight;
      }
      desFile << "\n";
    }
    // 补充没有出度的点
    for (vid_t v = 0; v < this->node_num; v++) {
      if (out_degree.find(v) == out_degree.end()
          || out_degree[v] == 0) {
        add_edges++;
        desFile << v << separator;
        vid_t rand_v = rand() % this->node_num + 0; // 生成[0, 5)范围内的随机数
        desFile << rand_v;
        if (FLAGS_weight) {
          desFile << separator << 1; // set default weight
        }
        desFile << "\n";
      }
    }
    LOG(INFO) << " add edges for zero out-degree: " << add_edges;
    desFile.close();

    LOG(INFO) << "  finish.";
  }

  void write_cut_file (int worker_num,
                       bool undirected,
                       const std::string out_dir,
                       const std::string out_edge = "") {
    LOG(INFO) << "start write_cut_file...";

    /* get range of each worker */
    std::string new_dir = out_dir + "/" + std::to_string(worker_num);
    std::string command = "mkdir -p " + new_dir; // 生成文件夹
    system(command.c_str());
    std::vector<int> node_range(worker_num+1, 0);
    int fnum_ = worker_num;
    size_t vnum = this->node_num;
    size_t frag_vnum = (vnum + fnum_ - 1) / fnum_;
    for (size_t i = 1; i < fnum_; ++i) {
      node_range[i] += node_range[i-1] + frag_vnum;
    }
    node_range[worker_num] = vnum;

    for (int i = 0; i < worker_num; i++) {
      LOG(INFO) << "worker_" << i << ": [" << node_range[i] <<"," 
                << node_range[i+1] << ")";
    }
    
    // 写入边文件
    for (int i = 0; i < worker_num; i++) {
      vid_t low = node_range[i];
      vid_t height = node_range[i+1];
      vid_t v_offset = low;
      LOG(INFO) << " low=" << low << ", height=" << height;

      const std::string separator=" ";
      std::ofstream desFile(new_dir + "/" + out_edge 
                            + "_" + std::to_string(i)
                            // + "#" + std::to_string(v_offset)
                            ); // 新文件id全部减去v_offset
      if ( !desFile ) {
        cout << "error opening destination file. " << out_edge << std::endl;
        desFile.close();
        return;
      }

      vid_t add_edges = 0;
      for (vid_t v = low; v < height; v++) {
        bool effected_edge = false;
        for (auto e : this->graph[v]) {
          if (e.to_vid >= low && e.to_vid < height) {
            desFile << (v - v_offset) << separator;
            desFile << (e.to_vid - v_offset);
            desFile << "\n";
            effected_edge = true;
          }
        }
        if (effected_edge == false) { // 如果切边导致没有出边，这加一条自环边
          add_edges++;
          desFile << (v - v_offset) << separator;
          desFile << (v - v_offset);
          desFile << "\n";
        }
      }
      LOG(INFO) << " add edges for zero out-degree: " << add_edges;
      desFile.close();
    }
    // 写入range
    std::ofstream desFile(new_dir + "/" + out_edge 
                          + "_readme"); // 新文件id划分数据
    if ( !desFile ) {
      cout << "error opening destination file. " << out_edge << std::endl;
      desFile.close();
      return;
    }
    for (auto rg : node_range) {
      desFile << rg << " ";
    }
    desFile.close();

    LOG(INFO) << "  finish.";
  }

  /**
   * 读文本文件的图.
  */
  void read_and_write_origin_1 (const std::string iefile, bool undirected = false,
                              const std::string out_edge = "") {
    std::ifstream srcFile( iefile, ios::in );
    if ( !srcFile ) {
      LOG(INFO) << "error opening source: " << iefile;
      exit(0);
    }
    LOG(INFO) << "start read_and_write_origin: " << iefile;

    char c[10000];
    for (int i = 0; i < FLAGS_head; i++) {
      srcFile.getline(c, 10000);
      LOG(INFO) << "ignore: " << c;
    }

    std::vector<std::pair<vid_t, Edge>> temp_graph;
    vid_t max_vid = 0;
    edge_num_t edge_num = 0;
    vid_t src_id = 0;
    vid_t to_id = 0;
    vid_t v_id = 0;
    std::set<vid_t> v_set;
    std::unordered_map<vid_t, vid_t> out_degree; // 统计每个点的出度
    while ( srcFile >> src_id ) {
      Edge e;
      srcFile >> e.to_vid;
      if (out_degree.find(src_id) == out_degree.end()) {
        out_degree[src_id] = 1;
      } else {
        out_degree[src_id] += 1;
      }
      v_set.insert(src_id);
      v_set.insert(e.to_vid);
      if (FLAGS_weight) {
        srcFile >> e.weight;
      }
      if (undirected && e.to_vid != src_id) {
        Edge ud_e(src_id, e.to_vid);
        temp_graph.emplace_back(std::pair<vid_t, Edge>(e.to_vid, ud_e));
        if (out_degree.find(e.to_vid) == out_degree.end()) {
          out_degree[e.to_vid] = 1;
        } else {
          out_degree[e.to_vid] += 1;
        }
      }
      temp_graph.emplace_back(std::pair<vid_t, Edge>(src_id, e));
    }
    srcFile.close();
    LOG(INFO) << "  read finish.";

    std::cout << " v_set.size()=" << v_set.size() << std::endl;

    this->node_num = v_set.size();
    this->edge_num = temp_graph.size();
    LOG(INFO) << " node_num=" << this->node_num;
    LOG(INFO) << " edge_num=" << this->edge_num;

    /*------------------------------------------------------------------------*/
    const std::string separator=" ";
    LOG(INFO) << "start write_graph: " << out_edge;
    LOG(INFO) << "  separator=" << separator;
    std::ofstream desFile(out_edge);
    if ( !desFile ) {
      cout << "error opening destination file. " << out_edge << std::endl;
      desFile.close();
      return;
    }

    vid_t add_edges = 0;
    // 原始边写入
    for (auto pair : temp_graph) {
      desFile << pair.first << separator;
      desFile << pair.second.to_vid;
      if (FLAGS_weight) {
        desFile << separator << pair.second.weight;
      }
      desFile << "\n";
    }
    // 补充没有出度的点
    std::vector<vid_t> v_array;
    v_array.assign(v_set.begin(), v_set.end());
    for (auto v : v_array) {
      if (out_degree.find(v) == out_degree.end()
          || out_degree[v] == 0) {
        add_edges++;
        desFile << v << separator;
        vid_t rand_dist = rand() % this->node_num + 0; // 生成[0, 5)范围内的随机数
        desFile << v_array[rand_dist];
        if (FLAGS_weight) {
          desFile << separator << 1; // set default weight
        }
        desFile << "\n";
      }
    }
    LOG(INFO) << " add edges for zero out-degree: " << add_edges;
    desFile.close();
    LOG(INFO) << "  write efile finish.";

    // 写入点文件
    std::ofstream vFile(out_edge + ".v");
    if ( !vFile ) {
      cout << "error opening destination file. " << out_edge << std::endl;
      vFile.close();
      return;
    }
    for (auto v : v_array) {
      vFile << v << "\n";
    }
    vFile.close();
    LOG(INFO) << "  write vfile finish.";

    LOG(INFO) << "  finish.";
  }

  /**
   * 读二进制文件的图
  */
  void read_graph_by_binary (const std::string iefile) {
    std::ifstream srcFile(iefile, ios::in | ios::binary);
    if ( !srcFile ) {
      LOG(INFO) << "error opening source: " << iefile;
      exit(0);
    }
    LOG(INFO) << "start read_graph_by_binary: " << iefile;

    std::vector<std::pair<vid_t, Edge>> temp_graph;
    vid_t max_vid = 0;
    edge_num_t edge_num = 0;
    vid_t src_id = 0;
    vid_t to_id = 0;
    temp_graph.reserve(936364282); // uk-2005
    while (srcFile.read( (char *) &src_id, sizeof(src_id))) {
      Edge e;
      srcFile.read((char*)&e.to_vid, sizeof(e.to_vid));
      max_vid = std::max(max_vid, src_id);
      max_vid = std::max(max_vid, e.to_vid);
      if (FLAGS_weight) {
        srcFile.read((char*)&e.weight, sizeof(e.weight));
      }
      // LOG(INFO) << src_id << " " << e.to_vid << ": " << e.weight;
      temp_graph.emplace_back(std::pair<vid_t, Edge>(src_id, e));
    }
    srcFile.close();

    this->node_num = max_vid + 1;
    this->edge_num = temp_graph.size();
    LOG(INFO) << " node_num=" << this->node_num;
    LOG(INFO) << " edge_num=" << this->edge_num;
    graph.resize(max_vid+1);
    if (FLAGS_hava_inadj) {
      graph_inadj.resize(max_vid+1);
    }
    for (auto p : temp_graph) {
      auto e = p.second;
      // LOG(INFO) << p.first << "->" << e.to_vid << " " << e.weight;
      graph[p.first].emplace_back(e);
      if (FLAGS_hava_inadj) {
        graph_inadj[e.to_vid].emplace_back(Edge(p.first, e.weight));
      }
    }
    LOG(INFO) << "  finish.";
  }

  /**
   * 将图写入二进制文件
  */
  void write_graph_to_binary (const std::string out_edge) {
    LOG(INFO) << "start write_graph_to_binary.";
    std::ofstream desFile( out_edge, ios::out | ios::binary );
    if ( !desFile ) {
      cout << " error opening destination file. " << out_edge << std::endl;
      desFile.close();
      return;
    }
    LOG(INFO) << "  start write_graph_to_binary: " << out_edge;

    vid_t write_edge_num = 0;
    for (vid_t i = 0; i < this->node_num; i++) {
      for (auto e : this->graph[i]) {
        // LOG(INFO) << i << "->" << e.to_vid << " " << e.weight;
        desFile.write((char *)&i, sizeof(i));
        desFile.write((char *)&e.to_vid, sizeof(e.to_vid));
        if (FLAGS_weight) {
          desFile.write((char *)&e.weight, sizeof(e.weight));
        }
        write_edge_num++;
      }
    }
    desFile.close();
    LOG(INFO) << "  write_edge_num=" << write_edge_num;
    LOG(INFO) << "finish write_graph_to_binary.";
  }

  /**
   * 将图写入文本文件
   *    如果没有出度的就随机指向一个顶点.
  */
  void write_graph_to_txt (const std::string out_edge, 
                           const std::string separator=" ") {
    LOG(INFO) << "start write_graph_to_txt: " << out_edge;
    LOG(INFO) << "  separator=" << separator;
    std::ofstream desFile(out_edge);
    if ( !desFile ) {
      cout << "error opening destination file. " << out_edge << std::endl;
      desFile.close();
      return;
    }

    vid_t add_edges = 0;
    for (vid_t i = 0; i < this->node_num; i++) {
      for (auto e : this->graph[i]) {
        desFile << i << separator;
        desFile << e.to_vid;
        if (FLAGS_weight) {
          desFile << separator << e.weight;
        }
        desFile << "\n";
      }
      if (this->graph[i].size() == 0) {
        add_edges++;
        desFile << i << separator;
        vid_t rand_dist = rand() % this->node_num + 0; // 生成[0, 5)范围内的随机数
        desFile << rand_dist;
        if (FLAGS_weight) {
          desFile << separator << 1; // set default weight
        }
        desFile << "\n";
      }
    }
    LOG(INFO) << " add edges for zero out-degree: " << add_edges;
    desFile.close();
    LOG(INFO) << "  finish.";
  }

  /**
   * 将顶点写入文本文件
   *    即: 0, 1, 2, ..., n-1
  */
  void write_V_to_txt (const std::string out_edge) {
    LOG(INFO) << "start write_V_to_txt: " << out_edge;
    std::ofstream desFile(out_edge);
    if ( !desFile ) {
      cout << "error opening destination file. " << out_edge << std::endl;
      desFile.close();
      return;
    }

    for (vid_t i = 0; i < this->node_num; i++) {
      desFile << i << "\n";
    }
    desFile.close();

    LOG(INFO) << "  finish. " << (out_edge);
  }

  /**
   * 有向图转无向图: 可能有重复边
  */
  void directedG_to_undirectedG () {
    LOG(INFO) << " 无向图:";
    LOG(INFO) << "directedG_to_undirectedG...";
    std::vector<std::vector<Edge> > graph_ud;
    graph_ud = this->graph;
    for (vid_t i = 0; i < this->node_num; i++) {
      for (auto e : this->graph[i]) {
        Edge r_e = e;
        r_e.to_vid = i;
        // graph_ud[e.to_vid].emplace_back(r_e);
        // 去重
        bool have = false;
        for (auto t_e : graph_ud[e.to_vid]) {
          if (t_e.to_vid == i) {
            have = true;
            break;
          }
        }
        if (!have) {
          graph_ud[e.to_vid].emplace_back(r_e);
        }
      }
    }
    this->graph = graph_ud;
    LOG(INFO) << "  graph -> graph_ud, finish.";
  }

  /**
   * 为边添加权重
  */
  void add_weight () {
    LOG(INFO) << "add_weight...";

    FLAGS_weight = true;
    // srand((unsigned)time(NULL));  // 关闭,便于调试
    for (vid_t i = 0; i < this->node_num; i++) {
      for (auto& e : this->graph[i]) {
        w_t w = rand() % 64 + 0; //生成[0, 64)范围内的随机数
        e.weight = w;
      }
    }
    LOG(INFO) << "  add_weight finish.";
  }


  /**
   * 读文本文件的图, 根据具体需要进行修改后写入文件
   *  将原图边权全部加1
  */
  void read_write_graph_by_txt (const std::string iefile, 
                                const std::string out_edge) {
    std::ifstream srcFile( iefile, ios::in );
    if ( !srcFile ) {
      LOG(INFO) << "error opening source: " << iefile;
      srcFile.close();
      exit(0);
    }
    LOG(INFO) << "start read_graph_by_txt: " << iefile;
    std::ofstream desFile(out_edge);
    if ( !desFile ) {
      cout << "error opening destination file. " << out_edge << std::endl;
      desFile.close();
      return;
    }
    LOG(INFO) << "start write_graph_by_txt: " << out_edge;

    vid_t src_id = 0;
    vid_t to_id = 0;
    while ( srcFile >> src_id ) {
      w_t weight;
      srcFile >> to_id;
      desFile << src_id << " " << to_id;
      if (FLAGS_weight) {
        srcFile >> weight;
        desFile << " " << (weight + 1);
      }
      desFile << "\n";
    }

    srcFile.close();
    desFile.close();
    LOG(INFO) << " finish read_write_graph_by_txt";
  }

  ~DealGraph(){
  }

  std::vector<std::vector<Edge> > graph;
  std::vector<std::vector<Edge> > graph_inadj;
  vid_t node_num = 0;
  edge_num_t edge_num = 0;
};

int main(int argc,char **argv) {
  /* 
    g++ deal_dataset.cc -lgflags -lglog -o deal_dataset && ./deal_dataset -weight=0 -base_edge=/home/yusong/dataset/large/cit-Patents/cit-Patents.txt -out_edge=/home/yusong/dataset/large/cit-Patents/cit-Patents.mtx2 -type=origin -head=3
  */
  auto start = std::chrono::system_clock::now();
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  FLAGS_logtostderr = true; //设置日志消息是否转到标准输出而不是日志文件, 即true：不写入文件，只在终端显示
  FLAGS_colorlogtostderr = true;  // Set log color

  std::string base_edge = FLAGS_base_edge;
  std::string out_edge = FLAGS_out_edge;
  LOG(INFO) << base_edge;

  DealGraph graph;

  bool undirected = false;
  if (base_edge.find("_ud.", 0) < base_edge.size()) {
    undirected = true;
    LOG(INFO) << " Identified to undirected graph...";
  }

  std::string type = FLAGS_type;
  LOG(INFO) << "FLAGS_type=" << FLAGS_type;

  if (FLAGS_type == "origin") {
    /**
      重新编号，并获得所有顶点升序列表
    */
    graph.read_and_write_origin(base_edge, undirected, out_edge);

  } else if (FLAGS_type == "reorder") {
    /**
      将顶点重新编号了，但是补充出度为0的边
    */
    // graph.read_graph_by_txt(base_edge); // 
    graph.read_graph_by_txt_renum(base_edge, undirected); // 重新编号，去调没用的id
    graph.write_graph_to_txt(out_edge + ".e"); // 写入边
    graph.write_V_to_txt(out_edge + ".v"); // 写入点
  } else if (FLAGS_type == "keep_order") {
    /**
      保持原图顶点大小顺序，将顶点重新编号成连续id，但是补充出度为0的边
    */
    LOG(INFO) << "keep_order!\n";
    // graph.read_graph_by_txt(base_edge); // 
    graph.read_and_write_keep_order(base_edge, undirected, out_edge); // 写入边
    // graph.write_V_to_txt(out_edge + ".v"); // 写入点
  } else if (FLAGS_type == "cut_file") {
    /**
      将原图进行划分，保证每个子图连续
    */
    graph.read_graph_by_txt(base_edge); // 
    graph.write_cut_file(FLAGS_worker_num, undirected, FLAGS_out_dir, out_edge); // 写入边
  } else {
    LOG(INFO) << "No match!\n";
  }

  google::ShutDownCommandLineFlags();
  google::ShutdownGoogleLogging();
  auto end = std::chrono::system_clock::now();
  fprintf(stderr, "time: %.6lfs\n", 1e-6*(uint64_t)std::chrono::duration_cast
                                <std::chrono::microseconds>(end-start).count());
  LOG(INFO) << "finish!\n";
  return 0;
}
