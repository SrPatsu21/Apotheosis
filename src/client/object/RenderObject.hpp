#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "../mash/Mesh.hpp"

class Material;

class RenderObject
{
private:
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;

    glm::mat4 modelMatrix{1.0f};
public:
    glm::vec3 position;
    glm::vec3 rotation; // Euler (radians)
    glm::vec3 scale;

    RenderObject(
        std::shared_ptr<Mesh> mesh,
        std::shared_ptr<Material> material,
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::vec3& rotation = glm::vec3(0.0f),
        const glm::vec3& scale    = glm::vec3(1.0f)
    );

    ~RenderObject() = default;

    RenderObject(const RenderObject&) = default;
    RenderObject& operator=(const RenderObject&) = default;

    RenderObject(RenderObject&&) noexcept = delete;
    RenderObject& operator=(RenderObject&&) noexcept = delete;

    const glm::vec3& getPosition() const { return position; }
    const glm::vec3& getRotation() const { return rotation; }
    const glm::vec3& getScale() const { return scale; }

    void updateModelMatrix();


    const std::shared_ptr<Mesh>& getMesh() const { return mesh; }
    const std::shared_ptr<Material>& getMaterial() const { return material; }
    const glm::mat4& getModelMatrix() const { return modelMatrix; }
};
