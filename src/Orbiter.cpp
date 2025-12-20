#include "Orbiter.h"

#include <imgui.h>

#include "App.h"

Orbiter::Orbiter(const glm::vec3& position, const float radius, const float aspect) :
    position(position), radius(radius), aspect(aspect) {
    ComputeProj();
    ComputeView();
}

void Orbiter::Update(const float dt) {
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

    ComputeView();
}

glm::mat4 Orbiter::View() const {
    return view;
}

glm::mat4 Orbiter::Proj() const {
    return proj;
}

void Orbiter::UI() const {
    ImGui::Text("Orbiter");
    ImGui::Text("Position %.2f %.2f %.2f", position.x, position.y, position.z);
    ImGui::Text("Radius   %.2f", radius);
    ImGui::Text("Pitch    %.2f", pitch);
    ImGui::Text("Yaw      %.2f", yaw);
}

void Orbiter::ComputeProj() {
    proj = glm::perspective(glm::radians(fov), aspect, nearP, farP);
}

void Orbiter::ComputeView() {
    const float camX = position.x + radius * glm::cos(pitch) * glm::sin(yaw);
    const float camY = position.y + radius * glm::sin(pitch);
    const float camZ = position.z + radius * glm::cos(pitch) * glm::cos(yaw);

    const glm::vec3 eye(camX, camY, camZ);
    constexpr glm::vec3 up(0.0f, 1.0f, 0.0f);

    view = glm::lookAt(eye, position, up);
}
