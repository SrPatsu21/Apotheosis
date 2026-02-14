#pragma once

#include "PushConstantObject.hpp"
#include "../RenderBatchManager.hpp"

#include <memory>
#include <glm/glm.hpp>

class RenderInstance
{
private:
    PushConstantObject modelMatrix;
public:
    RenderBatchManager::RenderBatch* ownerBatch = nullptr;
    size_t indexInBatch = 0;

    glm::vec3 position;
    glm::vec3 rotation; // Euler (radians)
    glm::vec3 scale;

    RenderInstance(
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::vec3& rotation = glm::vec3(0.0f),
        const glm::vec3& scale    = glm::vec3(1.0f)
    );

    ~RenderInstance() = default;

    RenderInstance(const RenderInstance&) = default;
    RenderInstance& operator=(const RenderInstance&) = default;

    RenderInstance(RenderInstance&&) noexcept = delete;
    RenderInstance& operator=(RenderInstance&&) noexcept = delete;

    const glm::vec3& getPosition() const { return position; }
    const glm::vec3& getRotation() const { return rotation; }
    const glm::vec3& getScale() const { return scale; }

    void updateModelMatrix();

    const PushConstantObject& getModelMatrix() const { return modelMatrix; }
};
