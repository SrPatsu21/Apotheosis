#pragma once

#include "../material/Material.hpp"

#include <vector>
#include <memory>
#include <glm/glm.hpp>

class RenderSkinnedBatch;
class RenderSkinnedBatchManager;
class Animator;

class SkinnedRenderInstance
{
    friend class RenderSkinnedBatch;
    friend class RenderSkinnedBatchManager;
public:
    struct BatchRegistration
    {
        RenderSkinnedBatch* batch = nullptr;
        size_t indexInBatch = 0;
        SkinnedRenderInstance* owner = nullptr;
    };
private:

    std::vector<BatchRegistration> registrations;

public:

    glm::vec3 position;
    glm::vec3 rotation; // Euler (radians)
    glm::vec3 scale;

    std::shared_ptr<Animator> animator;
    uint32_t boneOffset = 0;

    SkinnedRenderInstance(
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::vec3& rotation = glm::vec3(0.0f),
        const glm::vec3& scale = glm::vec3(0.0f)
    );

    ~SkinnedRenderInstance();

    SkinnedRenderInstance(const SkinnedRenderInstance&) = delete;
    SkinnedRenderInstance& operator=(const SkinnedRenderInstance&) = delete;

    SkinnedRenderInstance(SkinnedRenderInstance&&) noexcept = delete;
    SkinnedRenderInstance& operator=(SkinnedRenderInstance&&) noexcept = delete;

    void updateModelMatrix();

    void updateAnimation(float dt);

    std::vector<BatchRegistration> getRegistrations() const { return registrations; }

private:
    void addRegistration(
        RenderSkinnedBatch* batch,
        size_t index
    );

    void clearRegistrations();
};