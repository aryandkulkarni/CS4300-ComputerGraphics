#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() {
    position = glm::vec3(0.0f, 0.0f, 5.0f);
    direction = glm::vec3(0.0f, 0.0f, -1.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + direction, up);
}
