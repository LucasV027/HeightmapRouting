#pragma once

#include <memory>


#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

class OrbiterCamera : public Camera {
public:
    OrbiterCamera(const glm::vec3& position, float radius, float fovDeg, float aspect);

    void Update(float dt) override;

    static std::unique_ptr<Camera> Create(const glm::vec3& position, float radius, float fovDeg);

private:
    glm::vec3 ComputeEyePos() const;

private:
    glm::vec3 position;
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float radius;

    float yaw = 0.0;
    float pitch = glm::pi<float>() / 2.f;

    float moveSpeed = 100.0f;
    float rotateSpeed = 2.0f;
    float zoomSpeed = 100.0f;
};
