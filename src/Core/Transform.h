#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform {
public:
    explicit Transform(const glm::vec3& position = glm::vec3(0.0f),
                       const glm::quat& rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                       const glm::vec3& scale = glm::vec3(1.0f));

    void Translate(const glm::vec3& position);
    void Rotate(const glm::quat& rotation);
    void Scale(const glm::vec3& scale);
    void Scale(float uniformScale);

    const glm::mat4& GetMatrix() const;

    const glm::vec3& GetPosition() const { return position; }
    const glm::quat& GetRotation() const { return rotation; }
    const glm::vec3& GetScale() const { return scale; }

private:
    void UpdateMatrix() const;

private:
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    mutable glm::mat4 matrix = glm::mat4(1.0f);
    mutable bool dirty = true;
};
