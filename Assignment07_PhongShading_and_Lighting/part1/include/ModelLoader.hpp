#include <string>
#include <vector>
#include <glm/glm.hpp>

class ModelLoader {
public:
    bool loadOBJ(const std::string& path);
    const std::vector<float>& getVertexData() const;

    const std::vector<glm::vec3>& getVertices() const;
    const std::vector<unsigned int>& getIndices() const;

private:
    std::vector<float> vertexData;

    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
};
