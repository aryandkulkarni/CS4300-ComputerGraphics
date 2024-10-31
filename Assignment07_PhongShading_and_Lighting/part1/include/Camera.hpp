#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "glm/glm.hpp"

class Camera {
public:
    Camera();


    void MouseLook(int mouseX, int mouseY);
    void MoveForward(float speed);
    void MoveBackward(float speed);
    void MoveLeft(float speed);
    void MoveRight(float speed);
    void MoveUp(float speed);
    void MoveDown(float speed);

    void SetCameraEyePosition(float x, float y, float z);

    float GetEyeXPosition();
    float GetEyeYPosition();
    float GetEyeZPosition();

    float GetViewXDirection();
    float GetViewYDirection();
    float GetViewZDirection();

    glm::mat4 GetViewMatrix() const;

    glm::vec3 GetPosition() const { return m_eyePosition; }

private:
    glm::vec3 m_eyePosition;
    glm::vec3 m_viewDirection;
    glm::vec3 m_upVector;
    glm::vec2 m_oldMousePosition;
};

#endif
