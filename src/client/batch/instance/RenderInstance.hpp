#pragma once

#include "../RenderBatchManager.hpp"
#include "../material/Material.hpp"

#include <vector>
#include <memory>
#include <glm/glm.hpp>

class RenderInstance
{
    friend class RenderBatchManager;
public:
    struct BatchRegistration
    {
        RenderBatchManager::RenderBatch* batch = nullptr;
        size_t indexInBatch = 0;
        std::shared_ptr<Material> material;
    };
private:

    std::vector<BatchRegistration> registrations;

public:

    glm::vec3 position;
    glm::vec3 rotation; // Euler (radians)
    glm::vec3 scale;

    RenderInstance(
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::vec3& rotation = glm::vec3(0.0f),
        const glm::vec3& scale = glm::vec3(1.0f)
    );

    ~RenderInstance();

    RenderInstance(const RenderInstance&) = delete;
    RenderInstance& operator=(const RenderInstance&) = delete;

    RenderInstance(RenderInstance&&) noexcept = delete;
    RenderInstance& operator=(RenderInstance&&) noexcept = delete;

    void updateModelMatrix();

    std::vector<BatchRegistration> getRegistrations() const { return registrations; }

private:
    void addRegistration(
        RenderBatchManager::RenderBatch* batch,
        size_t index,
        std::shared_ptr<Material> material
    );

    void clearRegistrations();
};