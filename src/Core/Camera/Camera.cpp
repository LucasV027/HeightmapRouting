#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Core/App.h"

Camera::Camera(const float fovDeg, const float aspect) : fovDeg(fovDeg), aspect(aspect) {
}

void Camera::Update(float dt) {
    const auto& window = App::GetWindow();
    if (window.WasResized()) {
        aspect = window.GetAspectRatio();
        UpdateProj();
    }
}

void Camera::UpdateProj() {
    proj = glm::perspective(glm::radians(fovDeg), aspect, nearP, farP);
}

void Camera::UpdateView(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) {
    view = glm::lookAt(position, target, up);
}
