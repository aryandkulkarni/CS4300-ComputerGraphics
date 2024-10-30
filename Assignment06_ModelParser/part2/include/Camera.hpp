#pragma once
#include <glm/glm.hpp>

class Camera {
public:
    Camera();
    glm::mat4 getViewMatrix() const;

private:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up;
};
