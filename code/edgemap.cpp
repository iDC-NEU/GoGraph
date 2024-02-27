#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>

using namespace std;

// 读取顶点文件，返回顶点顺序
vector<int> readVertexOrder(const string& vertexFileName) {
    vector<int> vertexOrder;
    ifstream vertexFile(vertexFileName);

    if (vertexFile.is_open()) {
        int vertex;
        while (vertexFile >> vertex) {
            vertexOrder.push_back(vertex);
        }

        vertexFile.close();
    } else {
        cerr << "Unable to open vertex file: " << vertexFileName << endl;
    }

    return vertexOrder;
}

// 根据顶点顺序修改边文件
void mapEdges(const string& edgeFileName, const vector<int>& vertexOrder, const string& outputFileName) {
    unordered_map<int, int> vertexMap;
    for (size_t i = 0; i < vertexOrder.size(); ++i) {
        vertexMap[vertexOrder[i]] = i;
    }

    ifstream edgeFile(edgeFileName);
    ofstream outputFile(outputFileName);

    if (edgeFile.is_open() && outputFile.is_open()) {
        int vertex1, vertex2;
        while (edgeFile >> vertex1 >> vertex2) {
            outputFile << vertexMap[vertex1] << " " << vertexMap[vertex2] << endl;
        }

        edgeFile.close();
        outputFile.close();
    } else {
        cerr << "Unable to open input or output file." << endl;
    }
}

int main(int argc, char** argv) {
    // 顶点文件和边文件的文件名
    string vertexFileName = argv[1];
    string edgeFileName = argv[2];
    string outputFileName = vertexFileName + ".reorder.e";

    // 读取顶点文件
    vector<int> vertexOrder = readVertexOrder(vertexFileName);

    // 根据顶点顺序修改边文件
    mapEdges(edgeFileName, vertexOrder, outputFileName);

    cout << "Mapping completed. Output file: " << outputFileName << endl;

    return 0;
}
