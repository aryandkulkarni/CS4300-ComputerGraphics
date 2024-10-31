#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

class ModelLoader {
public:
    bool loadOBJ(const std::string& path);
    const std::vector<float>& getVertexData() const;

private:
    std::vector<float> vertexData;
};
