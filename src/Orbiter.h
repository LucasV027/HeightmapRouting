#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Orbiter {
public:
    explicit Orbiter(const glm::vec3& position, float radius, float aspect);

    void Update(float dt);

    glm::mat4 View() const;
    glm::mat4 Proj() const;

    const glm::vec3& Position() const { return position; }
    float Radius() const { return radius; }

    void UI() const;

private:
    void ComputeProj();
    void ComputeView();

private:
    glm::vec3 position;
    float radius;
    float aspect;

    float yaw = 0.0;
    float pitch = glm::pi<float>() / 2.f;

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);

    float fov = 45.0f;
    float nearP = 0.1f;
    float farP = 1000.0f;

    float moveSpeed = 100.0f;
    float rotateSpeed = 2.0f;
    float zoomSpeed = 100.0f;
};
