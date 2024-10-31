#include "ModelLoader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

bool ModelLoader::loadOBJ(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "OBJ file " << path << "could not be opened\n";
        return false;
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> vertexIndices, normalIndices;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        } else if (prefix == "vn") {
            glm::vec3 norm;
            ss >> norm.x >> norm.y >> norm.z;
            normals.push_back(norm);
        } else if (prefix == "f") {
            unsigned int vIdx[3], nIdx[3];
            char slash;
            for (int i = 0; i < 3; ++i) {
                ss >> vIdx[i] >> slash >> slash >> nIdx[i];
                vertexIndices.push_back(vIdx[i]);
                normalIndices.push_back(nIdx[i]);
            }
        }
    }

    for (size_t i = 0; i < vertexIndices.size(); ++i) {
        unsigned int vi = vertexIndices[i] - 1;
        unsigned int ni = normalIndices[i] - 1;

        glm::vec3 pos = positions[vi];
        glm::vec3 norm = normals[ni];
        glm::vec3 color = glm::vec3(0.8f, 0.8f, 0.8f);

        vertexData.insert(vertexData.end(), { pos.x, pos.y, pos.z });
        vertexData.insert(vertexData.end(), { color.r, color.g, color.b });
        vertexData.insert(vertexData.end(), { norm.x, norm.y, norm.z });
    }

    return true;
}

const std::vector<float>& ModelLoader::getVertexData() const {
    return vertexData;
}
