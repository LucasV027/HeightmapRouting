#include "FreeCamera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Core/App.h"

FreeCamera::FreeCamera(const glm::vec3& position,
                       const glm::vec3& orientation,
                       const float fovDeg,
                       const float aspect) :
    Camera(fovDeg, aspect), position(position), direction(glm::normalize(orientation)) {
    pitch = glm::degrees(std::asin(direction.y));
    yaw = glm::degrees(std::atan2(direction.z, direction.x));

    UpdateProj();
    UpdateView(position, position + direction, up);
}

void FreeCamera::Update(const float dt) {
    Camera::Update(dt);

    HandleInput(dt);
    HandleMouseLook();
    UpdateView(position, position + direction, up);
}

std::unique_ptr<Camera>
FreeCamera::Create(const glm::vec3& position, const glm::vec3& orientation, float fovDeg) {
    return std::make_unique<FreeCamera>(position, orientation, fovDeg,
                                        App::GetWindow().GetAspectRatio());
}

void FreeCamera::HandleInput(const float dt) {
    const auto& window = App::GetWindow();

    if (window.IsKeyInState(GLFW_KEY_C, GLFW_PRESS)) {
        cursorEnabled = !cursorEnabled;
        window.SetCursorMode(cursorEnabled ? Window::CursorMode::DISABLED
                                           : Window::CursorMode::NORMAL);
        firstMouseInput = true;
    }

    const glm::vec3 forward = glm::normalize(glm::vec3(direction.x, 0.0f, direction.z));
    const glm::vec3 right = glm::normalize(glm::cross(forward, up));

    float moveSpeed = speed * dt;

    // clang-format off
    if (window.IsKeyPressed(GLFW_KEY_LEFT_SHIFT))   moveSpeed *= boostSpeed;
    if (window.IsKeyPressed(GLFW_KEY_W))            position += forward * moveSpeed;
    if (window.IsKeyPressed(GLFW_KEY_S))            position -= forward * moveSpeed;
    if (window.IsKeyPressed(GLFW_KEY_D))            position += right * moveSpeed;
    if (window.IsKeyPressed(GLFW_KEY_A))            position -= right * moveSpeed;
    if (window.IsKeyPressed(GLFW_KEY_SPACE))        position += up * moveSpeed;
    if (window.IsKeyPressed(GLFW_KEY_LEFT_CONTROL)) position -= up * moveSpeed;
    // clang-format on
}

void FreeCamera::HandleMouseLook() {
    if (!cursorEnabled)
        return;

    const auto& window = App::GetWindow();
    auto [mouseX, mouseY] = window.GetMousePosition();

    if (firstMouseInput) {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        firstMouseInput = false;
        return;
    }

    const float xOffset = (mouseX - lastMouseX) * sensitivity;
    const float yOffset = (lastMouseY - mouseY) * sensitivity;
    lastMouseX = mouseX;
    lastMouseY = mouseY;

    yaw += xOffset;
    pitch = glm::clamp(pitch + yOffset, -89.0f, 89.0f);

    direction.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    direction.y = glm::sin(glm::radians(pitch));
    direction.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    direction = glm::normalize(direction);
}
