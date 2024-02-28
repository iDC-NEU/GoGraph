#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>

using namespace std;

// Read the vertex file and return the vertex order
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

// Modify edge files according to vertex order
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
    string vertexFileName = argv[1];
    string edgeFileName = argv[2];
    string outputFileName = vertexFileName + ".reorder.e";

    vector<int> vertexOrder = readVertexOrder(vertexFileName);

    mapEdges(edgeFileName, vertexOrder, outputFileName);

    cout << "Mapping completed. "<<endl;

    return 0;
}
