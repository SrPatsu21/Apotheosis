#pragma once
#include "../CoreVulkan.hpp"
#include "../swapchain&framebuffer/SwapchainManager.hpp"
#include "UniformBufferObject.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstring>
#include "../mash/BufferManager.hpp"

class CameraBufferManager
{
private:
    VkDevice device;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

public:

    struct ICameraProvider {
        virtual ~ICameraProvider() = default;

        virtual void fill(
            UniformBufferObject& ubo,
            float time,
            const VkExtent2D& extent
        ) = 0;
    };

    struct DefaultCameraProvider : ICameraProvider {
        void fill(
            UniformBufferObject& ubo,
            float time,
            const VkExtent2D& extent
        ) override;
    };

    CameraBufferManager(
        VkDevice device,
        BufferManager* bufferManager,
        int max_frames_in_flight
    );

    ~CameraBufferManager();

    void update(
        uint32_t currentFrame,
        const UniformBufferObject& ubo
    );

    const std::vector<VkBuffer>& getUniformBuffers() const { return uniformBuffers; }
    std::vector<VkDeviceMemory> getUniformBufferMemorys() const { return uniformBuffersMemory; }
    std::vector<void*> getUniformBuffersMapped() const {return uniformBuffersMapped; }
};