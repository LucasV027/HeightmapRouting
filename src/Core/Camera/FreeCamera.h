#pragma once

#include <memory>

#include "Camera.h"

class FreeCamera : public Camera {
public:
    FreeCamera(const glm::vec3& position, const glm::vec3& orientation, float fovDeg, float aspect);

    void Update(float dt) override;

    static std::unique_ptr<Camera>
    Create(const glm::vec3& position, const glm::vec3& orientation, float fovDeg);

private:
    void HandleInput(float dt);
    void HandleMouseLook();

private:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    float yaw = 0.0f;
    float pitch = 0.0f;

    float sensitivity = 0.1f;
    float speed = 20.0f;
    float boostSpeed = 3.5f;
    bool cursorEnabled = false;

    bool firstMouseInput = true;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
};
