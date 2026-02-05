#include "OrbiterCamera.h"

#include "Core/App.h"

OrbiterCamera::OrbiterCamera(const glm::vec3& position,
                             const float radius,
                             const float fovDeg,
                             const float aspect) :
    Camera(fovDeg, aspect), position(position), radius(radius) {

    UpdateProj();
    UpdateView(ComputeEyePos(), position, up);
}

void OrbiterCamera::Update(const float dt) {
    Camera::Update(dt);

    const auto& window = App::GetWindow();

    const glm::vec3 forward(glm::sin(yaw), 0.0f, glm::cos(yaw));
    const glm::vec3 right(glm::cos(yaw), 0.0f, -glm::sin(yaw));

    // clang-format off
    if (window.IsKeyPressed(GLFW_KEY_W)) position -= forward * moveSpeed * dt;
    if (window.IsKeyPressed(GLFW_KEY_S)) position += forward * moveSpeed * dt;
    if (window.IsKeyPressed(GLFW_KEY_A)) position -= right * moveSpeed * dt;
    if (window.IsKeyPressed(GLFW_KEY_D)) position += right * moveSpeed * dt;

    // Rotation
    if (window.IsKeyPressed(GLFW_KEY_LEFT))  yaw += rotateSpeed * dt;
    if (window.IsKeyPressed(GLFW_KEY_RIGHT)) yaw -= rotateSpeed * dt;
    if (window.IsKeyPressed(GLFW_KEY_UP))    pitch += rotateSpeed * dt;
    if (window.IsKeyPressed(GLFW_KEY_DOWN))  pitch -= rotateSpeed * dt;

    // Zoom
    if (window.IsKeyPressed(GLFW_KEY_KP_ADD))      radius -= zoomSpeed * dt;
    if (window.IsKeyPressed(GLFW_KEY_KP_SUBTRACT)) radius += zoomSpeed * dt;
    // clang-format on

    pitch = glm::clamp(pitch, -glm::half_pi<float>() + 0.1f, glm::half_pi<float>() - 0.1f);
    radius = glm::clamp(radius, 0.5f, 1000.0f);

    UpdateView(ComputeEyePos(), position, up);
}

std::unique_ptr<Camera>
OrbiterCamera::Create(const glm::vec3& position, float radius, float fovDeg) {
    return std::make_unique<OrbiterCamera>(position, radius, fovDeg,
                                           App::GetWindow().GetAspectRatio());
}

glm::vec3 OrbiterCamera::ComputeEyePos() const {
    return glm::vec3{
        position.x + radius * glm::cos(pitch) * glm::sin(yaw),
        position.y + radius * glm::sin(pitch),
        position.z + radius * glm::cos(pitch) * glm::cos(yaw),
    };
}
