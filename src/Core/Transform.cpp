#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>

Transform::Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) :
    position(position), rotation(rotation), scale(scale) {
}

void Transform::Translate(const glm::vec3& position) {
    this->position = position;
    dirty = true;
}

void Transform::Rotate(const glm::quat& rotation) {
    this->rotation = rotation;
    dirty = true;
}

void Transform::Scale(const glm::vec3& scale) {
    this->scale = scale;
    dirty = true;
}

void Transform::Scale(const float uniformScale) {
    scale = glm::vec3(uniformScale);
    dirty = true;
}

const glm::mat4& Transform::GetMatrix() const {
    if (dirty)
        UpdateMatrix();

    return matrix;
}

void Transform::UpdateMatrix() const {
    matrix = glm::translate(glm::mat4(1.0f), position);
    matrix *= glm::mat4_cast(rotation);
    matrix = glm::scale(matrix, scale);
    dirty = false;
}
