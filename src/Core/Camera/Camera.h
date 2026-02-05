#pragma once

#pragma once

#include <glm/glm.hpp>

class Camera {
public:
    Camera(float fovDeg, float aspect);
    virtual ~Camera() = default;

    virtual void Update(float dt);

    const glm::mat4& View() const { return view; }
    const glm::mat4& Proj() const { return proj; }

protected:
    void UpdateProj();
    void UpdateView(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up);

protected:
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);

    float fovDeg;
    float aspect;
    float nearP = 0.1f;
    float farP = 1000.0f;
};
