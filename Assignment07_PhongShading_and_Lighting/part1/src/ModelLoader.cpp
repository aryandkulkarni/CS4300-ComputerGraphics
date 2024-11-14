#include "ModelLoader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

bool ModelLoader::loadOBJ(const std::string& path) {
    std::ifstream objFile(path);
    if (!objFile.is_open()) {
        std::cerr << "Failed to open OBJ file: " << path << std::endl;
        return false;
    }

    std::vector<glm::vec3> temp_positions;
    std::vector<unsigned int> temp_indices;

    std::string line;
    while (std::getline(objFile, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 position;
            ss >> position.x >> position.y >> position.z;
            temp_positions.push_back(position);
        }
        else if (prefix == "f") {
            std::string vertexInfo;
            unsigned int vertexIndex;
            for (int i = 0; i < 3; ++i) {
                ss >> vertexInfo;
                std::stringstream vertexStream(vertexInfo);
                std::string indexStr;
                std::getline(vertexStream, indexStr, '/');
                vertexIdx = std::stoi(indexStr) - 1;
                temp_indices.push_back(vertexIdx);
            }
        }
    }
    vertices = temp_positions;
    indices = temp_indices;

    objFile.close();
    return true;
}

const std::vector<glm::vec3>& ModelLoader::getVertices() const {
    return vertices;
}

const std::vector<unsigned int>& ModelLoader::getIndices() const {
    return indices;
}

const std::vector<float>& ModelLoader::getVertexData() const {
    return vertexData;
}
